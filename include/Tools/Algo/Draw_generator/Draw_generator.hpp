/*!
 * \file
 * \brief Class tools::Draw_generator.
 */
#ifndef DRAW_GENERATOR_HPP_
#define DRAW_GENERATOR_HPP_

namespace aff3ct
{
namespace tools
{
template <typename R = float>
class Draw_generator
{
public:
	Draw_generator() = default;
	virtual ~Draw_generator() = default;
	virtual Draw_generator<R>* clone() const = 0;

	virtual void set_seed(const int seed) = 0;
};

}
}

#endif /* DRAW_GENERATOR_HPP_ */
