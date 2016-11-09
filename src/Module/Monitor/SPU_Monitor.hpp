#ifndef SPU_MONITOR_HPP_
#define SPU_MONITOR_HPP_

#ifdef STARPU
#include <vector>
#include <string>
#include <cassert>

#include <starpu.h>

#include "Tools/Perf/MIPP/mipp.h"

template <typename B>
class SPU_Monitor : public Monitor_i<B>
{
private:
	static starpu_codelet spu_cl_check_errors;

public:
	SPU_Monitor(const int K, const int N, const int n_frames = 1, const std::string name = "SPU_Monitor")
	: Monitor_i<B>(K, N, n_frames, name) {}

	virtual ~SPU_Monitor() {}

	static inline starpu_task* spu_task_check_errors(SPU_Monitor<B> *monitor, starpu_data_handle_t &in_data1,
	                                                                          starpu_data_handle_t &in_data2)
	{
		auto task = starpu_task_create();

		task->cl          = &SPU_Monitor<B>::spu_cl_check_errors;
		task->cl_arg      = (void*)(monitor);
		task->cl_arg_size = sizeof(*monitor);
		task->handles[0]  = in_data1;
		task->handles[1]  = in_data2;

		return task;
	}

private:
	static starpu_codelet spu_init_cl_check_errors()
	{
		starpu_codelet cl;

		cl.type              = STARPU_SEQ;
		cl.cpu_funcs     [0] = SPU_Monitor<B>::spu_kernel_check_errors;
		cl.cpu_funcs_name[0] = "Monitor::check_errors";
		cl.nbuffers          = 2;
		cl.modes         [0] = STARPU_R;
		cl.modes         [1] = STARPU_R;

		return cl;
	}

	static void spu_kernel_check_errors(void *buffers[], void *cl_arg)
	{
		auto monitor = static_cast<SPU_Monitor<B>*>(cl_arg);

		auto U_K = static_cast<mipp::vector<B>*>((void*)STARPU_VECTOR_GET_PTR(buffers[0]));
		auto V_K = static_cast<mipp::vector<B>*>((void*)STARPU_VECTOR_GET_PTR(buffers[1]));

		monitor->check_errors(*U_K, *V_K);
	}
};

template <typename B>
starpu_codelet SPU_Monitor<B>::spu_cl_check_errors = SPU_Monitor<B>::spu_init_cl_check_errors();

template <typename B>
using Monitor = SPU_Monitor<B>;
#else
template <typename B>
using Monitor = Monitor_i<B>;
#endif

#endif /* SPU_MONITOR_HPP_ */