#!/usr/bin/python

import sys
import re


DASM_ARGS = {
    "c": ("%s", "get_memadr(opcode, 'c')"),
    "d": ("%s", "get_memadr(opcode, 'd')"),
    "i": ("%02x", "opcode & 0xff"),
}

TYPES = {
    "":   None,
    "d":  None,
    "b":  "  cs->branch = BR_UB;",
    "cb": "  cs->branch = BR_CB;",
    "i":  "  cs->branch = BR_IDLE;",
    "f":  None,
}

def expand_c(v):
    fmt =  ["%s", "(%s & 0xffff0000)", "(%s << 16)"][v["crm"]]
    param = ["cmem[i->param]", "cmem[ca]"][v["cmode"]]
    return fmt %  param

def expand_d(v):
    index = ["(i->param + ", "(id + "][v["dmode"]]
    mask =  ["ba0) & 0xff] << 8)", "ba1) & 0x1f] << 8)"][v["dbp"]]
    return "(dmem%d[" % v["dbp"] + index + mask

def expand_d24(v):
    index = ["(i->param + ", "(id + "][v["dmode"]]
    mask =  ["ba0) & 0xff]", "ba1) & 0x1f]"][v["dbp"]]
    return "dmem%d[" % v["dbp"] + index + mask

EXPAND_ML = ["macc", "(macc << 2)", "(macc << 4)", "(macc >> 16)" ]

def expand_mv(v):
    c = ["", "s"][v["movm"]]
    return "check_macc_overflow_%d%s()" % (v["sfmo"], c)

EXPAND_WC = ["cmem[i->param] =", "cmem[ca] ="]


ROUNDING = [ 0, 1 << (48-32-1), 1 << (48-24-1), 1 << (48-30-1),
             1 << (48-16-1)]

A = (1 << 64) - 1
RMASK= [A,
        A - (1 << (48-32)) + 1,
        A - (1 << (48-24)) + 1,
        A - (1 << (48-30)) + 1,
        A - (1 << (48-16)) + 1,
        ]

def expand_mo(v):
    c = ["", "s"][v["movm"]]
    return "macc_to_output_%d%s(0x%016xULL, 0x%016xULL)" % (
        v["sfmo"], c, ROUNDING[v["rnd"]], RMASK[v["rnd"]])


def expand_wd1(v):
    index = ["(i->param + ", "(id + "][v["dmode"]]
    mask =  ["ba0) & 0xff] =", "ba1) & 0x1f] ="][v["dbp"]]
    return "dmem%d[" % v["dbp"] + index + mask

WA2 = (
"  if(r < -0x80000000 || r > 0x7fffffff)\n"
"    st1 |= ST1_AOV;\n"
"  aacc = r;")


PDESC_EXPAND = {
    "a":     lambda v: ["aacc", "(aacc << 7)"][v["sfao"]],
    "c":     expand_c,
    "d":     expand_d,
    "d24":   expand_d24,
    "i":     lambda v: "i->param",

    "ml":    lambda v: EXPAND_ML[v["sfma"]],
    "mo":    expand_mo,
    "mv":    expand_mv,
    "wa1":   lambda v: "r =",
    "wa2":   lambda v: WA2,
    "wc1":   lambda v: EXPAND_WC[v["cmode"]],
    "wd1":   expand_wd1,
    "b1":    lambda v: "pc = ",
    "b2":    lambda v: "  sti |= S_BRANCH;",
    "sfai1": lambda v: ["",  "((INT32)("][v["sfai"]],
    "sfai2": lambda v: ["",  ")) >> 1"][v["sfai"]],
}

PDESC = {
    "a":    (0, ["sfao"]),
    "c":    (0, ["cmode", "crm"]),
    "d":    (0, ["dmode", "dbp"]),
    "d24":  (0, ["dmode", "dbp"]),
    "i":    (0, []),
    "ml":   (0, ["sfma"]),
    "mo":   (0, ["sfmo", "rnd", "movm"]),
    "mv":   (0, ["sfmo", "movm"]),
    "wa":   (1, []),
    "wc":   (1, ["cmode"]),
    "wd":   (1, ["dmode", "dbp"]),
    "b":    (1, []),
    "sfai": (2, ["sfai"]),
}

VARIANTS = {
    "cmode": (2, "xmode(opcode, 'c', cs)" ),
    "dmode": (2, "xmode(opcode, 'd', cs)" ),
    "sfai":  (2, "sfai(st1)"),
    "crm":   (3, "crm(st1)"),
    "dbp":   (2, "dbp(st1)"),
    "sfao":  (2, "sfao(st1)"),
    "sfmo":  (4, "sfmo(st1)"),
    "rnd":   (5, "rnd(st1)"),
    "movm":  (2, "movm(st1)"),
    "sfma":  (4, "sfma(st1)"),
    # dummy
    "post":  (1, None),

}

VARIANT_CANONICAL_ORDER = [
    "cmode",
    "dmode",
    "sfai",
    "crm",
    "dbp",
    "sfao",
    "sfmo",
    "rnd",
    "movm",
    "sfma",
]

def EmitWithPrefix(f, out, prefix):
    for o in out:
        print >>f, prefix + o

class Instruction:

    def __init__(self, line):
        token = line.split()
        if len(token) == 5:
            token.append("")
        assert len(token) == 6

        self._name = token[0]
        self._cat = token[1]
        self._id = int(token[2], 16)
        self._cyc = int(token[3])
        self._rep = token[4]
        self._type = token[5]
        self._flags = set()
        if self._cat == "2b":
            self._flags.add("post")
        self._dasm = None
        self._run = []
        self._variants = 1
        # sanity checks
        assert 0 <= self._id
        if self._cat == "1":
            assert self._id < 0x40
        else:
            assert self._id < 0x80
        assert self._type in TYPES

    def __str__(self):
        return repr([self._name, self._cat, self._id, self._variants, self._flags])


    def GetDasmInfo(self):
        lst = []
        def extract(match):
            s = match.group()[1:]
            assert s in DASM_ARGS
            fmt, val = DASM_ARGS[s]
            lst.append(val)
            return fmt
        s = re.sub("%.", extract, self._dasm)
        return s, lst


    def GetCdecSum(self):
        lst = []
        n = 1
        for f in VARIANT_CANONICAL_ORDER:
            if f in self._flags:
                c, s = VARIANTS[f]
                lst.append(" + ")
                if n != 1:
                    lst.append("%d*" % n)
                lst.append(s)
                n *=c
        return "".join(lst)


    def EmitDasm(self, f, prefix):
        opcode, args = self.GetDasmInfo()
        args = [", " + a for a in args]
        print >>f, "%scase 0x%02x:" % (prefix, self._id)
        print >>f, "%s  sprintf(buf, \"%s\"%s);" % (prefix, opcode, "".join(args))
        print >>f, "%s  break;" % prefix


    def EmitCdec(self, f, prefix, no, empty):
        print >>f, "%scase 0x%02x: // %s" % (prefix, self._id, self._name)
        if not empty:
            print >>f, "%s  *op = %s%s;" % (prefix, no, self.GetCdecSum())
            if self._type == "f":
                for l in self._run:
                    print >>f, prefix + l
            else:
                l = TYPES[self._type]
                if l:
                    print >>f, prefix + l
        print >>f, "%s  break;" % prefix


    def ExpandCintrp(self, line, values):
        def extract(match):
            s = match.group()[1:]
            assert s in PDESC_EXPAND
            # call the right expand_XXX function
            return PDESC_EXPAND[s](values)


        return re.sub("%[a-z0-9]+", extract, line)


    def PreprocessRunString(self):
        out = []
        for r in self._run:
            if "%wa(" in r:
                assert r.endswith(");")
                r = r[0:-2].replace("%wa(", "%wa1 ") + ";"
                out.append(r)
                out.append("%wa2")
            elif "%wd(" in r:
                assert r.endswith(");")
                r = r[0:-2].replace("%wd(", "%wd1 ") + ";"
                out.append(r)
            elif "%wc(" in r:
                assert r.endswith(");")
                r = r[0:-2].replace("%wc(", "%wc1 ") + ";"
                out.append(r)
            elif "%b(" in r:
                assert r.endswith(");")
                r = r[0:-2].replace("%b(", "%b1") + ";"
                out.append(r)
                out.append("%b2")
            elif "%sfai(" in r:
                assert r.endswith(");")
                r = r[0:-2].replace("%sfai(", "")
                r = r.replace(",", " = %sfai1", 1)
                out.append(r + "%sfai2;")
            else:
                out.append(r)
        return out


    def EmitCintrpRecurse(self, f, prefix, no, flags_fixed, flags_unfixed):
        if not flags_unfixed:
            vals = []
            for v in  VARIANT_CANONICAL_ORDER:
                if v in flags_fixed:
                    #print "@@@@", f
                    vals.append("%s=%d" % (v, flags_fixed[v]))
            out = []
            out.append("case %d: // %s %s" % (no, self._name, " ".join(vals)))
            for line in self.PreprocessRunString():
                out.append(self.ExpandCintrp(line, flags_fixed))
            out.append("  break;")
            out.append("")
            EmitWithPrefix(f, out, prefix)
            return no + 1
        x = flags_unfixed.pop(-1)
        n = VARIANTS[x][0]
        for i in range(n):
            flags_fixed[x] = i
            no = self.EmitCintrpRecurse(f, prefix, no, flags_fixed, flags_unfixed)
        flags_unfixed.append(x)
        return no


    def EmitCintrp(self, f, prefix, no):
        if not self._run:
            return no
        flags = [fn for fn in VARIANT_CANONICAL_ORDER
                 if fn in self._flags]
        return self.EmitCintrpRecurse(f, prefix, no, {}, flags)


    def Finalize(self):
        def extract(match):
            s = match.group()[1:]
            assert s in PDESC
            self._flags.update(PDESC[s][1])
            # result does not matter
            return "X"
        for line in self._run:
            # ignore result of substitution
            re.sub("%[a-z0-9]+", extract, line)
        for f in self._flags:
            self._variants *= VARIANTS[f][0]


    def AddInfo(self, line):
        if self._dasm is None:
            self._dasm = line.lstrip()
        else:
            self._run.append(line)


def ins_cmp_dasm(a, b):
    if a._cat[0] != b._cat[0]:
        return cmp(a._cat[0], b._cat[0])
    else:
        return cmp(a._id, b._id)


def LoadLst(filename):
    instructions = []
    ins = None
    for n, line in enumerate(open(filename, "rU")):
        line = line.rstrip()
        if not line and ins:
            # new lines separate intructions
            ins.Finalize()
            ins = None
        elif line[0] in [" ", "\t"]:
            assert ins
            ins.AddInfo(line)
        else:
            ins = Instruction(line)
            instructions.append(ins)
    if ins:
        ins.Finalize()
    return instructions


def EmitDasm(f, ins_list):
    ins_list.sort(cmp=ins_cmp_dasm)
    last_cat = ""
    for i in ins_list:
        cat = i._cat[0]
        if cat != last_cat:
            if last_cat:
                print >>f, "#endif"
                print >>f
            print >>f, "#ifdef DASM" + cat
            last_cat = cat
        i.EmitDasm(f, "    ")
        print >>f
    print >>f, "#endif"
    print >>f


def EmitCdec(f, ins_list):
    ins_list.sort(cmp=ins_cmp_dasm)
    no = 4
    last_cat = ""
    for i in ins_list:
        if not i._run: continue
        cat = i._cat.upper()
        if cat == "2B": cat = "2A"
        if cat == "3": continue

        if cat != last_cat:
            if last_cat:
                print >>f, "#endif"
                print >>f
            print >>f, "#ifdef CDEC" + cat
            last_cat = cat

        i.EmitCdec(f, "", no, i._cat == "2b")
        no += i._variants
        print >>f

    no = 4
    for i in ins_list:
        if not i._run: continue
        cat = i._cat.upper()
        if cat == "2A": cat = "2B"

        if cat == "1":
            no += i._variants
            continue

        if cat != last_cat:
            if last_cat:
                print >>f, "#endif"
                print >>f
            print >>f, "#ifdef CDEC" + cat
            last_cat = cat

        i.EmitCdec(f, "", no, i._cat == "2a")
        no += i._variants
        print >>f
    print >>f, "#endif"
    print >>f

def EmitCintrp(f, ins_list):
    ins_list.sort(cmp=ins_cmp_dasm)
    print >>f, "#ifdef CINTRP"
    no = 4
    for i in ins_list:
        no = i.EmitCintrp(f, "", no)
    print >>f, "#endif"



ins_list = LoadLst(sys.argv[1])
try:
    f = open(sys.argv[2], "w")
except Exception, err:
    logging.error("cannot write file %s [%s]", fname, err)
    sys.exit(1)

EmitDasm(f, ins_list)
EmitCdec(f, ins_list)
EmitCintrp(f, ins_list)
