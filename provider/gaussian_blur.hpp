#ifndef user_gaussian_blur_hpp
#define user_gaussian_blur_hpp

#include "puzzler/puzzles/gaussian_blur.hpp"


class GaussianBlurProvider
  : public puzzler::GaussianBlurPuzzle
{
public:
  GaussianBlurProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::GaussianBlurInput *input,
				puzzler::GaussianBlurOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
