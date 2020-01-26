/*!
 * \file
 * \brief Class module::Adaptor_1_to_n.
 */
#ifndef CONNECTOR_1_TO_N_HPP_
#define CONNECTOR_1_TO_N_HPP_

#include <typeindex>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include "Module/Task.hpp"
#include "Module/Socket.hpp"
#include "Module/Adaptor/Adaptor.hpp"

namespace aff3ct
{
namespace module
{

class Adaptor_1_to_n : public Adaptor
{
public:
	inline Task&   operator[](const adp::tsk         t);
	inline Socket& operator[](const adp::sck::push_1 s);
	inline Socket& operator[](const adp::sck::pull_n s);

	const bool active_waiting;

	std::shared_ptr<std::vector<std::condition_variable>> cnd_pull;
	std::shared_ptr<std::vector<std::mutex             >> mtx_pull;
	std::shared_ptr<            std::condition_variable > cnd_put;
	std::shared_ptr<            std::mutex              > mtx_put;

public:
	inline Adaptor_1_to_n(const size_t n_elmts,
	                      const std::type_index datatype,
	                      const size_t buffer_size = 1,
	                      const bool active_waiting = true,
	                      const int n_frames = 1);
	inline Adaptor_1_to_n(const std::vector<size_t> &n_elmts,
	                      const std::vector<std::type_index> &datatype,
	                      const size_t buffer_size = 1,
	                      const bool active_waiting = true,
	                      const int n_frames = 1);
	virtual ~Adaptor_1_to_n() = default;
	virtual Adaptor_1_to_n* clone() const;

	inline void init();

	void wake_up();
	void cancel_waiting();

protected:
	virtual void push_1(const std::vector<const int8_t*> &in,  const int frame_id = -1);
	virtual void pull_n(const std::vector<      int8_t*> &out, const int frame_id = -1);
};
}
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "Module/Adaptor/Adaptor_1_to_n.hxx"
#endif

#endif /* CONNECTOR_1_TO_N_HPP_ */
