#ifndef user_rank_hpp
#define user_rank_hpp

#include "puzzler/puzzles/rank.hpp"


class RankProvider
  : public puzzler::RankPuzzle
{
public:
  RankProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::RankInput *input,
				puzzler::RankOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
