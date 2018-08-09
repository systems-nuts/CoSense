/*
	Authored 2018, Zhengyang Gu
 
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
#include <sys/time.h>

#ifdef NoisyOsLinux
#include <time.h>
#endif

#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "flextypes.h"
#include "flexerror.h"
#include "flex.h"
#include "common-errors.h"
#include "version.h"
#include "noisy-irPass-llvmBackends"
#include "common-data-structures.h"
#include "noisy-parser.h"
#include "noisy-lexer.h"
#include "common-irPass-helpers.h"
#include "noisy-types.h"

char *
nyTypeToLlvm(IrNode node)
{
	switch(node->type)
	{
		case kNoisyIrNodeType_Tbool:
		case kNoisyIrNodeType_T:
		case kNoisyIrNodeType_T:
		{
			return "i8";
		}
	}
}

void
emit 
LlvmBackendState *
llvmBackendStateInit()
{
	LlvmBackendState *  Nl;

	Nl = (LlvmBackendState *) calloc(1, sizeof(LlvmBackendState));
	if (Nl == NULL)
	{
		fatal(NULL, Emalloc);
	}
	return Nl;
}

void
irPassLlvmRegisterFunc(State *  N, LlvmBackendState *  Nl, IrNode *  node)
{
	Nl->nFuncs++;
	Nl->funcs = (NoisyFunc **) realloc(Nl->funcs, Nl->nFuncs * sizeof(NoisyFunc *));

	if (Nl->funcs == NULL)
	{
		fatal(NULL, Emalloc);
	}

	Nl->func[Nl->nFuncs] = (NoisyFunc *) calloc(1, sizeof(NoisyFunc));
	currentFunc = Nl->func[Nl->nFuncs];

	if (currentFunc == NULL)
	{
		fatal(NULL, Emalloc);
	}

	currentFunc->name = L(node)->tokenString;

}

NoisyADT *
irPassLlvmEmitFuncIOTypes(State *  N, LlvmBackendState *  Nl, IrNode *  node, NoisyFunc *  currentFunc, bool isInput)
{
	if(node->type != kNoisyIrNodeType_PtupleType)
	{
		fatal(N, EtokenUnrecognized);
	}

	NoisyADT *  currentADT;
	char *  varName;

	if (isInput)
	{
		currentFunc->inputVar = ;
		varName = currentFunc->inputVar;
		currentFunc->inputADT = (NoisyADT *) calloc(1, sizeof(NoisyADT));
		currentADT = currentFunc->inputADT;
	}
	else
	{
		currentFunc->outputVar = ;
		varName = currentFunc->outputVar;
		currentFunc->outputADT = (NoisyADT *) calloc(1, sizeof(NoisyADT));
		currentADT = currentFunc->outputADT;
	}

	if (currentADT == NULL)
	{
		fatal(NULL, Emalloc);
	}

	asprintf(&(currentADT->name), "%s.%s", currentFunc->name, varName);

}

void
irPassLlvmEmitHeader(State *  N)
{
	flexprint(N->Fe, N->Fm, N->Fpllvm, "source_filename = \"%s\"", N->fileName);
	flexprint(N->Fe, N->Fm, N->Fpllvm, "target datalayout = \"<target-data-layout>\"\n");
	flexprint(N->Fe, N->Fm, N->Fpllvm, "target triple = \"<target-triple>\"\n\n");

	return;
}

void
irPassLlvmEmitProgTypeNameDecl(State *  N, LlvmBackendState *  Nl, IrNode *  node)
{
	switch(R(node)->type)
	{
		case kNoisyIrNodeType_TrealConst:
		{

			flexprint(N->Fe, N->Fm, N->Fpllvm, "@%s = constant float %x, align 4\n",
				L(node)->identifier, *(unsigned int*)&(R(node)->realConst));
			break;
		}
		case kNoisyIrNodeType_TintConst:
		{
			flexprint(N->Fe, N->Fm, N->Fpllvm, "@%s = constant int %d, align 4\n",
				L(node)->identifier, *(unsigned int*)&(R(node)->intConst));
			break;
		}
		case kNoisyIrNodeType_TboolConst:
		{
			flexprint(N->Fe, N->Fm, N->Fpllvm, "@%s = constant i8 %d, align 4\n",
				L(node)->identifier, *(unsigned int*)&(R(node)->intConst));
			break;
		}
		case kNoisyIrNodeType_PnamegenDeclaration:
		{
			
			break;
		}
		case kNoisyIrNodeType_PtypeDeclaration:
		{
			Nl->nStructs++;
			Nl->structs = (NoisyADT **) realloc(Nl->structs, Nl->nStructs * sizeof(NoisyADT *));
			
			if (Nl->firstStruct == NULL)
			{
				fatal(NULL, Emalloc);
			}

			Nl->structs[Nl->nStructs - 1] = (NoisyADT *) calloc(1, sizeof(NoisyADT));
			NoisyADT *	currentStruct = Nl->structs[Nl->nStructs - 1];

			if (currentStruct == NULL)
			{
				fatal(NULL, Emalloc);
			}

			currentStruct->name = L(node)->tokenString;
			flexprint(N->Fe, N->Fm, N->Fpllvm, "%%struct.%s = type {", L(node)->tokenString);
			
			if(L(R(node))->type != kNoisyIrNodeType_PadtTypeDeclaration)
			{
				fatal(N, EtokenUnrecognized);
			}
			
			currentStruct->nFields = 0;

			/*
			 * We first iterrate over the tree once to get the number of fields before we
			 * can malloc
			 */
			for (IrNode *	current = L(R(node); current != NULL; current = R(R(current)))
			{
				currentStruct->nFields++;
			}

			currentStruct->fields = (char **)calloc(1, sizeof(char *) * currentStruct->nFields);

			/*
			 * For an ADT, we store its field name under llvmState, and write the type of its
			 * fields in the LLVM file, e.g.
			 * for 
	 		 * Pixel : adt
	   		   {
				r : int8;
				g : int8;
				b : int8;
				a : int8;
	   		   };
			 * ["r", "g", "b", "a"] will be stored under N->backendState->structs, and 
			 * `%struct.Pixel = type { i8, i8, i8, i8 }` will be written to the output file.
			 * 
			 * Since LLVM IR does not permit extra comma after the last field, and that Noisy
			 * ADTs must have at least one field, we treat the first field seperately.
			 */

			IrNode *	currentNode = L(R(node);
			currentStruct->fields[0] = currentNode->tokenString;
			flexprint(N->Fe, N->Fm, N->Fpllvm, " %s", nyTypeToLlvm(L(R(currentNode))));
			currentNode = R(R(currentNode));

			for (int i = 1; i < currentStruct->nFields; ++i)
			{
				currentStruct->fields[i] = currentNode->tokenString;
				flexprint(N->Fe, N->Fm, N->Fpllvm, ", %s", nyTypeToLlvm(L(R(currentNode))));
			}

			flexprint(N->Fe, N->Fm, N->Fpllvm, " }\n");
			break;
		}
		default:
		{
			fatal(N, )
		}
	}

}

void
irPassLlvmEmitProgtypeBody(State *  N, LlvmBackendState *  Nl, IrNode *  node)
{
	if (node->type != kNoisyIrNodeType_PprogtypeBody)
	{
		fatal(N, EtokenUnrecognized);
	}
	for(IrNode *	current = node; current != NULL; current = R(current))
	{
		irPassLlvmEmitProgTypeNameDecl(N, L(N));
	}

	return;
}

void
irPassLlvmEmitProgType(State *  N, LlvmBackendState *  Nl, IrNode *  node)
{
	if(node->type != kNoisyIrNodeType_PprogtypeDeclaration)
	{
		fatal(N, EtokenUnrecognized);
	}
	Nl->module = L(node)->identifier;
	flexprint(N->Fe, N->Fm, N->Fpllvm, "; ModuleID = %s\n", L(node)->identifier);

	irPassLlvmEmitProgtypeBody(N, R(node));

	return;
}

void
irPassLlvmEmitNameGen(State *  N, LlvmBackendState *  Nl, IrNode *  node)
{
	if(node->type != kNoisyIrNodeType_PnamegenDefinition)
	{
		fatal(N, EtokenUnrecognized);
	}

	Nl->currentFunc = L(node);


}

void
irPassLlvmEmitProgram(State *  N, IrNode *  node)
{
	LlvmBackendState *	Nl = llvmBackendStateInit();
	N->backendState = Nl;

	if(node->type != kNoisyIrNodeType_Pprogram)
	{
		fatal(N, EtokenUnrecognized);
	}
	irPassLlvmEmitHeader();
	irPassLlvmEmitProgType(N, Nl, L(node));

	for (IrNode *  current = R(node); current != NULL; current = R(current))
	{
		irPassLlvmEmitNameGen(N, Nl, L(current));
	}
}