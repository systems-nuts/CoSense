/*
	Authored 2015. Phillip Stanley-Marbell.

	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	*	Redistributions of source code must retain the above
		copyright notice, this list of conditions and the following
		disclaimer.

	*	Redistributions in binary form must reproduce the above
		copyright notice, this list of conditions and the following
		disclaimer in the documentation and/or other materials
		provided with the distribution.

	*	Neither the name of the author nor the names of its
		contributors may be used to endorse or promote products
		derived from this software without specific prior written
		permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
	FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <stdint.h>
#include "flextypes.h"
#include "flexerror.h"
#include "flex.h"
#include "common-errors.h"
#include "noisy-timeStamps.h"
#include "common-timeStamps.h"
#include "common-data-structures.h"
#include "common-lexers-helpers.h"
#include "common-firstAndFollow.h"

/*
 *	NOTE: Unlike in our previous compilers (e.g., Crayon), we do
 *
 *		inFirst(IrNodeType productionOrToken)
 *
 *	with the token being checked implicit in the lexer state, rather than
 *
 *		inFirst(IrNodeType productionOrToken, IrNodeType token)
 */
bool
inFirst(State *  N, IrNodeType productionOrToken, int firsts[kCommonIrNodeTypeMax][kCommonIrNodeTypeMax], int maxKey)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyInFirst);

	Token *	token = lexPeek(N, 1);

	if (productionOrToken > kCommonIrNodeTypeMax)
	{
		fatal(N, Esanity);
	}

	/*
	 *	NOTE: The arrays created by ffi2code have a maxKey element at the end of each sub-array
	 */
	for (int i = 0; i < maxKey && firsts[productionOrToken][i] != maxKey; i++)
	{
		if (firsts[productionOrToken][i] == token->type)
		{
			return true;
		}
	}

	return false;
}


/*
 *	NOTE: Unlike in our previous compilers (e.g., Crayon), we do
 *
 *		inFollow(IrNodeType productionOrToken)
 *
 *	with the token being checked implicit in the lexer state, rather than
 *
 *		inFollow(IrNodeType productionOrToken, IrNodeType token)
 */
bool
inFollow(State *  N, IrNodeType productionOrToken, int follows[kCommonIrNodeTypeMax][kCommonIrNodeTypeMax], int maxKey)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyInFollow);

	Token *	token = lexPeek(N, 1);

	if (productionOrToken > kCommonIrNodeTypeMax)
	{
		fatal(N, Esanity);
	}

	/*
	 *	NOTE: The arrays created by ffi2code have a maxKey element at the end of each sub-array
	 */
	for (int i = 0; i < maxKey && follows[productionOrToken][i] != maxKey; i++)
	{
		if (follows[productionOrToken][i] == token->type)
		{
			return true;
		}
	}

	return false;
}
