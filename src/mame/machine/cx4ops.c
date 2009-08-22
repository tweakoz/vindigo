#ifdef CX4_C

//Sprite Functions
void CX4_op00(running_machine* machine)
{
	switch(cx4.reg[0x4d])
	{
		case 0x00: CX4_op00_00(machine); break;
		case 0x03: CX4_op00_03(); break;
		case 0x05: CX4_op00_05(machine); break;
		case 0x07: CX4_op00_07(); break;
		case 0x08: CX4_op00_08(machine); break;
		case 0x0b: CX4_op00_0b(machine); break;
		case 0x0c: CX4_op00_0c(machine); break;
	}
}

//Draw Wireframe
void CX4_op01(running_machine* machine)
{
	memset(cx4.ram + 0x300, 0, 2304);
	CX4_C4DrawWireFrame(machine);
}

//Propulsion
void CX4_op05(running_machine *machine)
{
	INT32 temp = 0x10000;
	if(CX4_readw(0x1f83))
	{
		temp = CX4_sar((temp / CX4_readw(0x1f83)) * CX4_readw(0x1f81), 8);
	}
	CX4_writew(machine, 0x1f80, temp);
}

//Set Vector length
void CX4_op0d(running_machine *machine)
{
	cx4.C41FXVal    = CX4_readw(0x1f80);
	cx4.C41FYVal    = CX4_readw(0x1f83);
	cx4.C41FDistVal = CX4_readw(0x1f86);
	cx4.tanval = sqrt(((double)cx4.C41FYVal) * ((double)cx4.C41FYVal) + ((double)cx4.C41FXVal) * ((double)cx4.C41FXVal));
	cx4.tanval = (double)cx4.C41FDistVal / cx4.tanval;
	cx4.C41FYVal = (INT16)(((double)cx4.C41FYVal * cx4.tanval) * 0.99);
	cx4.C41FXVal = (INT16)(((double)cx4.C41FXVal * cx4.tanval) * 0.98);
	CX4_writew(machine, 0x1f89, cx4.C41FXVal);
	CX4_writew(machine, 0x1f8c, cx4.C41FYVal);
}

//Triangle
void CX4_op10()
{
	cx4.r0 = CX4_ldr(0);
	cx4.r1 = CX4_ldr(1);

	cx4.r4 = cx4.r0 & 0x1ff;
	if(cx4.r1 & 0x8000)
	{
		cx4.r1 |= ~0x7fff;
	}

	CX4_mul(CX4_cos(cx4.r4), cx4.r1, &cx4.r5, &cx4.r2);
	cx4.r5 = (cx4.r5 >> 16) & 0xff;
	cx4.r2 = (cx4.r2 << 8) + cx4.r5;

	CX4_mul(CX4_sin(cx4.r4), cx4.r1, &cx4.r5, &cx4.r3);
	cx4.r5 = (cx4.r5 >> 16) & 0xff;
	cx4.r3 = (cx4.r3 << 8) + cx4.r5;

	CX4_str(0, cx4.r0);
	CX4_str(1, cx4.r1);
	CX4_str(2, cx4.r2);
	CX4_str(3, cx4.r3);
	CX4_str(4, cx4.r4);
	CX4_str(5, cx4.r5);
}

//Triangle
void CX4_op13()
{
	cx4.r0 = CX4_ldr(0);
	cx4.r1 = CX4_ldr(1);

	cx4.r4 = cx4.r0 & 0x1ff;

	CX4_mul(CX4_cos(cx4.r4), cx4.r1, &cx4.r5, &cx4.r2);
	cx4.r5 = (cx4.r5 >> 8) & 0xffff;
	cx4.r2 = (cx4.r2 << 16) + cx4.r5;

	CX4_mul(CX4_sin(cx4.r4), cx4.r1, &cx4.r5, &cx4.r3);
	cx4.r5 = (cx4.r5 >> 8) & 0xffff;
	cx4.r3 = (cx4.r3 << 16) + cx4.r5;

	CX4_str(0, cx4.r0);
	CX4_str(1, cx4.r1);
	CX4_str(2, cx4.r2);
	CX4_str(3, cx4.r3);
	CX4_str(4, cx4.r4);
	CX4_str(5, cx4.r5);
}

//Pythagorean
void CX4_op15(running_machine *machine)
{
	double temp = 0.0;
	cx4.C41FXVal = CX4_readw(0x1f80);
	cx4.C41FYVal = CX4_readw(0x1f83);
	temp = sqrt((double)cx4.C41FXVal * (double)cx4.C41FXVal + (double)cx4.C41FYVal * (double)cx4.C41FYVal);
	cx4.C41FDist = (INT16)temp;
	CX4_writew(machine, 0x1f80, cx4.C41FDist);
}

//Calculate distance
void CX4_op1f(running_machine *machine)
{
	cx4.C41FXVal = CX4_readw(0x1f80);
	cx4.C41FYVal = CX4_readw(0x1f83);
	if(!cx4.C41FXVal)
	{
		cx4.C41FAngleRes = (cx4.C41FYVal > 0) ? 0x080 : 0x180;
	}
	else
	{
    	cx4.tanval = ((double)cx4.C41FYVal) / ((double)cx4.C41FXVal);
    	cx4.C41FAngleRes = (INT16)(atan(cx4.tanval) / (PI * 2) * 512);
    	cx4.C41FAngleRes = cx4.C41FAngleRes;
    	if(cx4.C41FXVal < 0)
    	{
      		cx4.C41FAngleRes += 0x100;
    	}
		cx4.C41FAngleRes &= 0x1ff;
	}
	CX4_writew(machine, 0x1f86, cx4.C41FAngleRes);
}

//Trapezoid
void CX4_op22()
{
	INT16 angle1 = CX4_readw(0x1f8c) & 0x1ff;
	INT16 angle2 = CX4_readw(0x1f8f) & 0x1ff;
	INT32 tan1 = CX4_Tan(angle1);
	INT32 tan2 = CX4_Tan(angle2);
	INT16 y = CX4_readw(0x1f83) - CX4_readw(0x1f89);
	INT16 left, right;
	INT32 j;

	for(j = 0; j < 225; j++, y++)
	{
		if(y >= 0)
		{
			left  = CX4_sar((INT32)tan1 * y, 16) - CX4_readw(0x1f80) + CX4_readw(0x1f86);
			right = CX4_sar((INT32)tan2 * y, 16) - CX4_readw(0x1f80) + CX4_readw(0x1f86) + CX4_readw(0x1f93);

			if(left < 0 && right < 0)
			{
				left  = 1;
				right = 0;
			}
			else if(left < 0)
			{
				left  = 0;
			}
			else if(right < 0)
			{
				right = 0;
			}

			if(left > 255 && right > 255)
			{
				left  = 255;
				right = 254;
			}
			else if(left > 255)
			{
				left  = 255;
			}
			else if(right > 255)
			{
				right = 255;
			}
		}
		else
		{
			left  = 1;
			right = 0;
		}
		cx4.ram[j + 0x800] = (UINT8)left;
		cx4.ram[j + 0x900] = (UINT8)right;
	}
}

//Multiply
void CX4_op25()
{
	cx4.r0 = CX4_ldr(0);
	cx4.r1 = CX4_ldr(1);
	CX4_mul(cx4.r0, cx4.r1, &cx4.r0, &cx4.r1);
	CX4_str(0, cx4.r0);
	CX4_str(1, cx4.r1);
}

//Transform Coords
void CX4_op2d(running_machine *machine)
{
	cx4.C4WFXVal  = CX4_readw(0x1f81);
	cx4.C4WFYVal  = CX4_readw(0x1f84);
	cx4.C4WFZVal  = CX4_readw(0x1f87);
	cx4.C4WFX2Val = CX4_read (0x1f89);
	cx4.C4WFY2Val = CX4_read (0x1f8a);
	cx4.C4WFDist  = CX4_read (0x1f8b);
	cx4.C4WFScale = CX4_readw(0x1f90);
	CX4_C4TransfWireFrame2();
	CX4_writew(machine, 0x1f80, cx4.C4WFXVal);
	CX4_writew(machine, 0x1f83, cx4.C4WFYVal);
}

//Sum
void CX4_op40()
{
	UINT32 i;
	cx4.r0 = 0;
	for(i=0;i<0x800;i++)
	{
		cx4.r0 += cx4.ram[i];
	}
	CX4_str(0, cx4.r0);
}

//Square
void CX4_op54()
{
	cx4.r0 = CX4_ldr(0);
	CX4_mul(cx4.r0, cx4.r0, &cx4.r1, &cx4.r2);
	CX4_str(1, cx4.r1);
	CX4_str(2, cx4.r2);
}

//Immediate Register
void CX4_op5c()
{
	CX4_str(0, 0x000000);
	CX4_immediate_reg(0);
}

//Immediate Register (Multiple)
void CX4_op5e() { CX4_immediate_reg( 0); }
void CX4_op60() { CX4_immediate_reg( 3); }
void CX4_op62() { CX4_immediate_reg( 6); }
void CX4_op64() { CX4_immediate_reg( 9); }
void CX4_op66() { CX4_immediate_reg(12); }
void CX4_op68() { CX4_immediate_reg(15); }
void CX4_op6a() { CX4_immediate_reg(18); }
void CX4_op6c() { CX4_immediate_reg(21); }
void CX4_op6e() { CX4_immediate_reg(24); }
void CX4_op70() { CX4_immediate_reg(27); }
void CX4_op72() { CX4_immediate_reg(30); }
void CX4_op74() { CX4_immediate_reg(33); }
void CX4_op76() { CX4_immediate_reg(36); }
void CX4_op78() { CX4_immediate_reg(39); }
void CX4_op7a() { CX4_immediate_reg(42); }
void CX4_op7c() { CX4_immediate_reg(45); }

//Immediate ROM
void CX4_op89()
{
	CX4_str(0, 0x054336);
	CX4_str(1, 0xffffff);
}

#endif
