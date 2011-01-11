/************************************************************************
 *
 *  MAME - Discrete sound system emulation library
 *
 *  Written by Keith Wilkins (mame@esplexo.co.uk)
 *
 *  (c) K.Wilkins 2000
 *
 *  Coding started in November 2000
 *  KW - Added Sawtooth waveforms  Feb2003
 *
 ***********************************************************************
 *
 * SEE DISCRETE.H for documentation on usage
 *
 ***********************************************************************
 *
 * Each sound primative DSS_xxxx or DST_xxxx has its own implementation
 * file. All discrete sound primatives MUST implement the following
 * API:
 *
 * dsX_NAME_step(inputs, context, float timestep)  - Perform time step
 *                                                  return output value
 * dsX_NAME_reset(context) - Reset to initial state
 *
 * Core software takes care of traversing the netlist in the correct
 * order
 *
 * DEVICE_START(discrete)      - Read Node list, initialise & reset
 * DEVICE_STOP(discrete)       - Shutdown discrete sound system
 * DEVICE_RESET(discrete)      - Put sound system back to time 0
 * discrete_stream_update() - This does the real update to the sim
 *
 ************************************************************************/

#include "emu.h"
#include "streams.h"
#include "wavwrite.h"
#include "discrete.h"


/*************************************
 *
 *  Performance
 *
 *************************************/

/*
 * Normally, the discrete core processes 960 samples per update.
 * With the various buffers involved, this on a Core2 is not as
 * performant as processing 240 samples 4 times.
 * The setting most probably depends on CPU and which modules are
 * run and how many tasks are defined.
 *
 * Values < 32 exhibit poor performance (too much overhead) while
 * Values > 500 have a slightly worse performace (too much cache misses?).
 */

#define MAX_SAMPLES_PER_TASK_SLICE	(240)

/*************************************
 *
 *  Debugging
 *
 *************************************/

#define DISCRETE_DEBUGLOG			(0)


/*************************************
 *
 *  Profiling Nodes
 *
 *************************************/

#define DISCRETE_PROFILING			(0)

/*************************************
 *
 *  Prototypes
 *
 *************************************/

//static DEVICE_RESET( discrete );
//static STREAM_UPDATE( discrete_stream_update );
static STREAM_UPDATE( buffer_stream_update );

static int profiling = 0;

/*************************************
 *
 *  Debug logging
 *
 *************************************/

void CLIB_DECL ATTR_PRINTF(2,3) discrete_device::discrete_log(const char *text, ...) const
{
	if (DISCRETE_DEBUGLOG)
	{
		va_list arg;
		va_start(arg, text);

		if(m_disclogfile)
		{
			vfprintf(m_disclogfile, text, arg);
			fprintf(m_disclogfile, "\n");
			fflush(m_disclogfile);
		}

		va_end(arg);
	}
}

/*************************************
 *
 *  INLINEs
 *
 *************************************/

INLINE void linked_list_tail_add(const discrete_device *info, linked_list_entry ***list_tail_ptr, const void *ptr)
{
	**list_tail_ptr = auto_alloc(info->machine, linked_list_entry);
	(**list_tail_ptr)->ptr = ptr;
	(**list_tail_ptr)->next = NULL;
	*list_tail_ptr = &((**list_tail_ptr)->next);
}

INLINE int linked_list_count(const linked_list_entry *list)
{
	int cnt = 0;
	const linked_list_entry *entry;

	for (entry = list; entry != NULL; entry = entry->next)
		cnt++;

	return cnt;
}

INLINE void linked_list_add(const discrete_device *info, linked_list_entry **list, const void *ptr)
{
	linked_list_entry *entry;

	if (*list == NULL)
	{
		*list = auto_alloc(info->machine, linked_list_entry);
		(*list)->ptr = ptr;
		(*list)->next = NULL;
	}
	else
	{
		for (entry = *list; entry != NULL && entry->next != NULL; entry = entry->next)
			;
		entry->next = auto_alloc(info->machine, linked_list_entry);
		entry->next->ptr = ptr;
		entry->next->next = NULL;
	}
}

/*************************************
 *
 *  Included simulation objects
 *
 *************************************/

#include "disc_sys.c"		/* discrete core modules and support functions */
#include "disc_wav.c"		/* Wave sources   - SINE/SQUARE/NOISE/etc */
#include "disc_mth.c"		/* Math Devices   - ADD/GAIN/etc */
#include "disc_inp.c"		/* Input Devices  - INPUT/CONST/etc */
#include "disc_flt.c"		/* Filter Devices - RCF/HPF/LPF */
#include "disc_dev.c"		/* Popular Devices - NE555/etc */



/*************************************
 *
 *  Master module list
 *
 *************************************/

inline void discrete_device::step_nodes_in_list(node_list_t &list)
{

	if (EXPECTED(!profiling))
	{
		for_each(discrete_base_node *, entry, &list)
		{
			/* Now step the node */
			entry.item()->step();
		}
	}
	else
	{
		osd_ticks_t last = get_profile_ticks();

		for_each(discrete_base_node *, entry, &list)
		{
			discrete_base_node *node = entry.item();

			node->run_time -= last;
			node->step();
			last = get_profile_ticks();
			node->run_time += last;
		}
	}
}

/*************************************
 *
 *  Find a given node
 *
 *************************************/

discrete_base_node *discrete_device::discrete_find_node(int node)
{
	if (node < NODE_START || node > NODE_END) return NULL;
	return m_indexed_node[NODE_INDEX(node)];
}

/*************************************
 *
 *  Build import list
 *
 *************************************/

void discrete_device::discrete_build_list(const discrete_sound_block *intf, linked_list_entry ***current)
{
	int node_count = 0;

	for (; intf[node_count].type != DSS_NULL; )
	{
		/* scan imported */
		if (intf[node_count].type == DSO_IMPORT)
		{
			discrete_log("discrete_build_list() - DISCRETE_IMPORT @ NODE_%02d", NODE_INDEX(intf[node_count].node) );
			discrete_build_list((discrete_sound_block *) intf[node_count].custom, current);
		}
		else if (intf[node_count].type == DSO_REPLACE)
		{
			linked_list_entry *entry;

			node_count++;
			if (intf[node_count].type == DSS_NULL)
				fatalerror("discrete_build_list: DISCRETE_REPLACE at end of node_list");

			for (entry = m_block_list; entry != NULL; entry = entry->next)
			{
				discrete_sound_block *block = (discrete_sound_block *) entry->ptr;

				if (block->type != NODE_SPECIAL )
					if (block->node == intf[node_count].node)
					{
						entry->ptr = (void *) &intf[node_count];
						discrete_log("discrete_build_list() - DISCRETE_REPLACE @ NODE_%02d", NODE_INDEX(intf[node_count].node) );
						break;
					}
			}

			if (entry == NULL)
				fatalerror("discrete_build_list: DISCRETE_REPLACE did not found node %d", NODE_INDEX(intf[node_count].node));

		}
		else if (intf[node_count].type == DSO_DELETE)
		{
			linked_list_entry *entry, *last;

			last = NULL;
			for (entry = m_block_list; entry != NULL; last = entry, entry = entry->next)
			{
				discrete_sound_block *block = (discrete_sound_block *) entry->ptr;

				if ((block->node >= intf[node_count].input_node[0]) &&
						(block->node <= intf[node_count].input_node[1]))
				{
					discrete_log("discrete_build_list() - DISCRETE_DELETE deleted NODE_%02d", NODE_INDEX(block->node) );
					if (last != NULL)
						last->next = entry->next;
					else
						m_block_list = entry->next;
				}
			}
		}
		else
		{
			discrete_log("discrete_build_list() - adding node %d (*current %p)\n", node_count, *current);
			linked_list_tail_add(this, current, &intf[node_count]);
		}

		node_count++;
	}
}

/*************************************
 *
 *  Sanity check list
 *
 *************************************/

void discrete_device::discrete_sanity_check(void)
{
	const linked_list_entry *entry;
	int node_count = 0;

	discrete_log("discrete_start() - Doing node list sanity check");
	for (entry = m_block_list; entry != NULL; entry = entry->next)
	{
		discrete_sound_block *block = (discrete_sound_block *) entry->ptr;

		/* make sure we don't have too many nodes overall */
		if (node_count > DISCRETE_MAX_NODES)
			fatalerror("discrete_start() - Upper limit of %d nodes exceeded, have you terminated the interface block?", DISCRETE_MAX_NODES);

		/* make sure the node number is in range */
		if (block->node < NODE_START || block->node > NODE_END)
			fatalerror("discrete_start() - Invalid node number on node %02d descriptor", block->node);

		/* make sure the node type is valid */
		if (block->type > DSO_OUTPUT)
			fatalerror("discrete_start() - Invalid function type on NODE_%02d", NODE_INDEX(block->node) );

		/* make sure this is a main node */
		if (NODE_CHILD_NODE_NUM(block->node) > 0)
			fatalerror("discrete_start() - Child node number on NODE_%02d", NODE_INDEX(block->node) );

		node_count++;
	}
	discrete_log("discrete_start() - Sanity check counted %d nodes", node_count);

}

/*************************************
 *
 *  Master discrete system start
 *
 *************************************/


/*************************************
 *
 *  Master discrete system stop
 *
 *************************************/

static UINT64 list_run_time(const node_list_t &list)
{
	UINT64 total = 0;

	for_each(discrete_base_node *, node, &list)
	{
		total += node.item()->run_time;
	}
	return total;
}

void discrete_device::display_profiling(void)
{
	int count;
	UINT64 total;
	UINT64 tresh;
	double tt;

	/* calculate total time */
	total = list_run_time(m_node_list);
	count = m_node_list.count();
	/* print statistics */
	printf("Total Samples  : %16" I64FMT "d\n", m_total_samples);
	tresh = total / count;
	printf("Threshold (mean): %16" I64FMT "d\n", tresh / m_total_samples );
	for_each(discrete_base_node *, node, &m_node_list)
	{
		if (node.item()->run_time > tresh)
			printf("%3d: %20s %8.2f %10.2f\n", node.item()->index(), node.item()->module_name(), (float) node.item()->run_time / (float) total * 100.0, ((float) node.item()->run_time) / (float) m_total_samples);
	}

	/* Task information */
	for_each(discrete_task *, task, &task_list)
	{
		tt =  list_run_time(task.item()->list);

		printf("Task(%d): %8.2f %15.2f\n", task.item()->task_group, tt / (double) total * 100.0, tt / (double) m_total_samples);
	}

	printf("Average samples/stream_update: %8.2f\n", (double) m_total_samples / (double) m_total_stream_updates);
}



/*************************************
 *
 *  Master reset of all nodes
 *
 *************************************/

/*************************************
 *
 *  Stream update functions
 *
 *************************************/

static void *task_callback(void *param, int threadid)
{
	task_list_t *list = (task_list_t *) param;
	int samples;

	do
	{
		for_each(discrete_task *, task, list)
		{
			/* try to lock */
			if (task.item()->lock_threadid(threadid))
			{
				samples = MIN(task.item()->samples, MAX_SAMPLES_PER_TASK_SLICE);

				/* check dependencies */
				for_each(discrete_source_node *, sn, &task.item()->source_list)
				{
					int avail;

					avail = sn.item()->task->ptr[sn.item()->output_node] - sn.item()->ptr;
					assert_always(avail >= 0, "task_callback: available samples are negative");
					if (avail < samples)
						samples = avail;
				}

				task.item()->samples -= samples;
				assert_always(task.item()->samples >=0, "task_callback: task_samples got negative");
				while (samples > 0)
				{
					/* step */
					task.item()-> device->step_nodes_in_list(task.item()->list);
					samples--;
				}
				if (task.item()->samples == 0)
				{
					/* return and keep the task locked so it is not picked up by other worker threads */
					return NULL;
				}
				task.item()->unlock();
			}
		}
	} while (1);

	return NULL;
}


static STREAM_UPDATE( buffer_stream_update )
{
	const discrete_base_node *node = (discrete_base_node *) param;
	const struct dss_input_context *context = (struct dss_input_context *)node->context;
	stream_sample_t *ptr = outputs[0];
	int data = context->data;
	int samplenum = samples;

	while (samplenum-- > 0)
	  *(ptr++) = data;
}



/*************************************
 *
 *  First pass init of nodes
 *
 *************************************/


void discrete_device::init_nodes(const linked_list_entry *block_list)
{
	const linked_list_entry	*entry;
	discrete_task *task = NULL;
	/* list tail pointers */
	int					has_tasks = 0;

	/* check whether we have tasks ... */
	for (entry = block_list; entry != NULL; entry = entry->next)
	{
		const discrete_sound_block *block = (discrete_sound_block *) entry->ptr;
		if (block->type == DSO_TASK_START)
			has_tasks = 1;
	}

	if (!has_tasks)
	{
		/* make sure we have one simple task
         * No need to create a node since there are no dependencies.
         */
		task = auto_alloc_clear(machine, discrete_task(this));
		task_list.add_tail(task);
	}

	/* loop over all nodes */
	for (entry = block_list; entry != NULL; entry = entry->next)
	{
		const discrete_sound_block *block = (discrete_sound_block *) entry->ptr;
		//int modulenum;

		discrete_base_node *node = block->factory->Create(this, block);
		/* keep track of special nodes */
		if (block->node == NODE_SPECIAL)
		{
			switch(block->type)
			{
				/* Output Node */
				case DSO_OUTPUT:
					m_output_list.add_tail(node);
					break;

				/* CSVlog Node for debugging */
				case DSO_CSVLOG:
					break;

				/* Wavelog Node for debugging */
				case DSO_WAVELOG:
					break;

				/* Task processing */
				case DSO_TASK_START:
					if (task != NULL)
						fatalerror("init_nodes() - Nested DISCRETE_START_TASK.");
					task = auto_alloc_clear(machine, discrete_task(this));
					node->context = task;
					break;

				case DSO_TASK_END:
					if (task == NULL)
						fatalerror("init_nodes() - NO DISCRETE_START_TASK.");
					task->numbuffered = 0;
					task->task_group = -1; /* will be set later */
					task->source_list.reset();
					task_list.add_tail(task);
					node->context = task;
					//task = NULL;
					break;

				default:
					fatalerror("init_nodes() - Failed, trying to create unknown special discrete node.");
			}
		}

		/* otherwise, make sure we are not a duplicate, and put ourselves into the indexed list */
		else
		{
			if (m_indexed_node[NODE_INDEX(block->node)])
				fatalerror("init_nodes() - Duplicate entries for NODE_%02d", NODE_INDEX(block->node));
			m_indexed_node[NODE_INDEX(block->node)] = node;
		}

		/* if we are an stream input node, track that */
		if (block->type == DSS_INPUT_STREAM)
		{
			m_input_list.add_tail(node);
		}
		else if (block->type == DSS_INPUT_BUFFER)
		{
			m_input_list.add_tail(node);
		}

		/* add to node list */
		m_node_list.add_tail(node);

		/* our running order just follows the order specified */
		/* does the node step ? */
		if (node->is_stepping())
		{
			/* do we belong to a task? */
			if (task == NULL)
				fatalerror("init_nodes() - found node outside of task.");
			else
				task->list.add_tail(node);
		}

		if (block->type == DSO_TASK_END)
		{
			task = NULL;
		}

		/* and register save state */
		node->save_state(this);
	}

	if (!has_tasks)
	{
	}

	/* if no outputs, give an error */
	if (m_output_list.count() == 0)
		fatalerror("init_nodes() - Couldn't find an output node");
}


/*************************************
 *
 *  node_description implementation
 *
 *************************************/

discrete_base_node::discrete_base_node(discrete_device * pdev, const discrete_sound_block *xblock) :
	context(NULL),
	device(pdev),
	run_time(0),
	m_context_size(0),
	m_block(xblock)
{
	output[0] = 0.0;
	m_custom = m_block->custom;
	m_active_inputs = m_block->active_inputs;
}

discrete_base_node::~discrete_base_node(void)
{
	if (m_context_size)
		global_free(context);
}

void discrete_base_node::start(void)
{
	/* allocate memory if necessary */
	if (m_context_size)
		context =  global_alloc_array(UINT8, m_context_size);
}

int discrete_base_node::index(void)
{
	return NODE_INDEX(m_block->node);
}

void discrete_base_node::save_state(device_t *device)
{
	if (m_block->node != NODE_SPECIAL)
		state_save_register_device_item_array(device, m_block->node, output);
}

void discrete_base_node::find_input_nodes(void)
{
	int inputnum;

	/* loop over all active inputs */
	for (inputnum = 0; inputnum < m_active_inputs; inputnum++)
	{
		int inputnode = m_block->input_node[inputnum];

		/* if this input is node-based, find the node in the indexed list */
		if IS_VALUE_A_NODE(inputnode)
		{
			discrete_base_node *node_ref = device->m_indexed_node[NODE_INDEX(inputnode)];
			if (!node_ref)
				fatalerror("discrete_start - NODE_%02d referenced a non existent node NODE_%02d", index(), NODE_INDEX(inputnode));

			if ((NODE_CHILD_NODE_NUM(inputnode) >= node_ref->max_output()) /*&& (node_ref->module_type() != DST_CUSTOM)*/)
				fatalerror("discrete_start - NODE_%02d referenced non existent output %d on node NODE_%02d", index(), NODE_CHILD_NODE_NUM(inputnode), NODE_INDEX(inputnode));

			input[inputnum] = &(node_ref->output[NODE_CHILD_NODE_NUM(inputnode)]);	/* Link referenced node out to input */
			m_input_is_node |= 1 << inputnum;			/* Bit flag if input is node */
		}
		else
		{
			/* warn if trying to use a node for an input that can only be static */
			if IS_VALUE_A_NODE(m_block->initial[inputnum])
			{
				device->discrete_log("Warning - discrete_start - NODE_%02d trying to use a node on static input %d",  index(), inputnum);
				/* also report it in the error log so it is not missed */
				logerror("Warning - discrete_start - NODE_%02d trying to use a node on static input %d",  index(), inputnum);
			}
			else
			{
				input[inputnum] = &(m_block->initial[inputnum]);
				//node->input[inputnum] = getDoublePtr(node->block->initial[inputnum]);
			}
		}
	}
	for (inputnum = m_active_inputs; inputnum < DISCRETE_MAX_INPUTS; inputnum++)
	{
		/* FIXME: Check that no nodes follow ! */
		input[inputnum] = &(m_block->initial[inputnum]);
	}
}

int discrete_device::same_module_index(discrete_base_node &node)
{
	int index = 0;

	for_each(discrete_base_node *, n, &m_node_list)
	{
		if (n.item() == &node)
			return index;
		if (n.item()->module_type() == node.module_type())
			index++;
	}
	return -1;
}


//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

const device_type DISCRETE = discrete_device_config::static_alloc_device_config;

//**************************************************************************
//  DEVICE CONFIGURATION
//**************************************************************************

#if 0
//-------------------------------------------------
//  static_set_type - configuration helper to set
//  the chip type
//-------------------------------------------------

void discrete_device_config::static_set_type(device_config *device, int type)
{
	discrete_device_config *asc = downcast<discrete_device_config *>(device);
	asc->m_type = type;
}

//-------------------------------------------------
//  static_set_type - configuration helper to set
//  the IRQ callback
//-------------------------------------------------


void discrete_device_config::static_set_irqf(device_config *device, void (*irqf)(device_t *device, int state))
{
	discrete_device_config *asc = downcast<discrete_device_config *>(device);
	asc->m_irq_func = irqf;
}
#endif

//-------------------------------------------------
//  discrete_device_config - constructor
//-------------------------------------------------

discrete_device_config::discrete_device_config(const machine_config &mconfig, const char *tag, const device_config *owner, UINT32 clock)
	: device_config(mconfig, static_alloc_device_config, "DISCRETE", tag, owner, clock),
	  device_config_sound_interface(mconfig, *this)
{
}


//-------------------------------------------------
//  static_alloc_device_config - allocate a new
//  configuration object
//-------------------------------------------------

device_config *discrete_device_config::static_alloc_device_config(const machine_config &mconfig, const char *tag, const device_config *owner, UINT32 clock)
{
	return global_alloc(discrete_device_config(mconfig, tag, owner, clock));
}


//-------------------------------------------------
//  alloc_device - allocate a new device object
//-------------------------------------------------

device_t *discrete_device_config::alloc_device(running_machine &machine) const
{
	return auto_alloc(&machine, discrete_device(machine, *this));
}


//-------------------------------------------------
//  discrete_device - constructor
//-------------------------------------------------

discrete_device::discrete_device(running_machine &_machine, const discrete_device_config &config)
	: device_t(_machine, config),
	  device_sound_interface(_machine, config, *this),
	  m_config(config)
{
	//memset(&m_info, 0, sizeof(m_info));

}

discrete_device::~discrete_device(void)
{
	osd_work_queue_free(m_queue);

	if (profiling)
	{
		display_profiling();
	}

	/* Process nodes which have a stop func */

	for_each(discrete_base_node *, node, &m_node_list)
	{
		node.item()->stop();
	}

	if (DISCRETE_DEBUGLOG)
	{
		/* close the debug log */
	    if (m_disclogfile)
	    	fclose(m_disclogfile);
		m_disclogfile = NULL;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void discrete_device::device_start()
{
	// create the stream
	//m_stream = stream_create(this, 0, 2, 22257, this, static_stream_generate);

	linked_list_entry **intf;
	const discrete_sound_block *intf_start = (discrete_sound_block *) baseconfig().static_config();
	char name[32];

	/* If a clock is specified we will use it, otherwise run at the audio sample rate. */
	if (this->clock())
		m_sample_rate = this->clock();
	else
		m_sample_rate = this->machine->sample_rate;
	m_sample_time = 1.0 / m_sample_rate;
	m_neg_sample_time = - m_sample_time;

	m_total_samples = 0;
	m_total_stream_updates = 0;

	/* create the logfile */
	sprintf(name, "discrete%s.log", this->tag());
	if (DISCRETE_DEBUGLOG)
		m_disclogfile = fopen(name, "w");

	/* enable profiling */
	if (getenv("DISCRETE_PROFILING"))
		profiling = atoi(getenv("DISCRETE_PROFILING"));

	/* Build the final block list */
	m_block_list = NULL;
	intf = &m_block_list;
	discrete_build_list(intf_start, &intf);

	/* first pass through the nodes: sanity check, fill in the indexed_nodes, and make a total count */
	discrete_sanity_check();

	/* Start with empty lists */
	m_node_list.reset();
	m_output_list.reset();
	m_input_list.reset();

	/* allocate memory to hold pointers to nodes by index */
	m_indexed_node = auto_alloc_array_clear(this->machine, discrete_base_node *, DISCRETE_MAX_NODES);

	/* initialize the node data */
	init_nodes(m_block_list);

	/* now go back and find pointers to all input nodes */
	for_each(discrete_base_node *, node, &m_node_list)
	{
		node.item()->find_input_nodes();
	}

	/* initialize the stream(s) */
	discrete_stream = stream_create(this,m_input_list.count(), m_output_list.count(), m_sample_rate, this, static_stream_generate);

	/* allocate a queue */

	m_queue = osd_work_queue_alloc(WORK_QUEUE_FLAG_MULTI | WORK_QUEUE_FLAG_HIGH_FREQ);

	/* Process nodes which have a start func */

	for_each(discrete_base_node *, node, &m_node_list)
	{
		node.item()->start();
	}
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void discrete_device::device_reset()
{

	stream_update(discrete_stream);

	/* loop over all nodes */
	for_each (discrete_base_node *, node, &m_node_list)
	{
		node.item()->output[0] = 0;

		node.item()->reset();
	}
}

//-------------------------------------------------
//  stream_generate - handle update requests for
//  our sound stream
//-------------------------------------------------

STREAM_UPDATE( discrete_device::static_stream_generate )
{
	reinterpret_cast<discrete_device *>(param)->stream_generate(inputs, outputs, samples);
}

void discrete_device::stream_generate(stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	int outputnum;
	//, task_group;

	if (samples == 0)
		return;
	/* Setup any output streams */
	outputnum = 0;
	for_each(discrete_base_node *, node, &m_output_list)
	{
		node.item()->context = (void *) outputs[outputnum];
		outputnum++;
	}

	/* Setup any input streams */
	for_each(discrete_base_node *, node, &m_input_list)
	{
		struct dss_input_context *context = (struct dss_input_context *) node.item()->context;
		context->ptr = (stream_sample_t *) inputs[context->stream_in_number];
	}

	/* Setup tasks */
	for_each(discrete_task *, task, &task_list)
	{
		int					i;

		task.item()->samples = samples;
		/* unlock the thread */
		task.item()->unlock();

		/* set up task buffers */
		for (i = 0; i < task.item()->numbuffered; i++)
			task.item()->ptr[i] = task.item()->node_buf[i];

		/* initialize sources */
		for_each(discrete_source_node *, sn, &task.item()->source_list)
		{
			sn.item()->ptr = sn.item()->task->node_buf[sn.item()->output_node];
		}
	}

	for_each(discrete_task *, task, &task_list)
	{
		/* Fire a work item for each task */
		osd_work_item_queue(m_queue, task_callback, (void *) &task_list, WORK_ITEM_FLAG_AUTO_RELEASE);
	}
	osd_work_queue_wait(m_queue, osd_ticks_per_second()*10);

	if (profiling)
	{
		m_total_samples += samples;
		m_total_stream_updates++;
	}
}

//-------------------------------------------------
//  read - read from the chip's registers and internal RAM
//-------------------------------------------------

READ8_MEMBER( discrete_device::read )
{
	discrete_base_node *node = discrete_find_node(offset);

	UINT8 data = 0;

	/* Read the node input value if allowed */
	if (node)
	{
		/* Bring the system up to now */
		stream_update(discrete_stream);

		data = (UINT8) node->output[NODE_CHILD_NODE_NUM(offset)];
	}
	else
		fatalerror("discrete_sound_r read from non-existent NODE_%02d\n", offset-NODE_00);

    return data;
}

//-------------------------------------------------
//  write - write to the chip's registers and internal RAM
//-------------------------------------------------

WRITE8_MEMBER( discrete_device::write )
{
	discrete_base_node *node = discrete_find_node(offset);

	/* Update the node input value if it's a proper input node */
	if (node)
	{
		struct dss_input_context *context = (struct dss_input_context *)node->context;
		UINT8 new_data    = 0;

		switch (node->module_type())
		{
			case DSS_INPUT_DATA:
			case DSS_INPUT_BUFFER:
				new_data = data;
				break;
			case DSS_INPUT_LOGIC:
			case DSS_INPUT_PULSE:
				new_data = data ? 1 : 0;
				break;
			case DSS_INPUT_NOT:
				new_data = data ? 0 : 1;
				break;
		}

		if (context->data != new_data)
		{
			if (context->is_buffered)
			{
				/* Bring the system up to now */
				stream_update(context->buffer_stream);

				context->data = new_data;
			}
			else
			{
				/* Bring the system up to now */
				stream_update(discrete_stream);

				context->data = new_data;

				/* Update the node output here so we don't have to do it each step */
				node->output[0] = new_data * context->gain + context->offset;
			}
		}
	}
	else
	{
		discrete_log("discrete_sound_w write to non-existent NODE_%02d\n", offset-NODE_00);
	}
}
