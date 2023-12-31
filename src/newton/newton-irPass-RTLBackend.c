/*
	Authored 2019. Vasileios Tsoutsouras.

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

#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

#include "flextypes.h"
#include "flexerror.h"
#include "flex.h"
#include "common-errors.h"
#include "version.h"
#include "newton-timeStamps.h"
#include "common-timeStamps.h"
#include "common-data-structures.h"
#include "noisy-parser.h"
#include "newton-parser.h"
#include "noisy-lexer.h"
#include "newton-lexer.h"
#include "common-irPass-helpers.h"
#include "common-lexers-helpers.h"
#include "common-irHelpers.h"
#include "newton-types.h"
#include "newton-symbolTable.h"

#define Q_PARAMETER 15
#define N_PARAMETER 32
//#define ICE40

static char multiplierVersion[32] = "qmultSequential";
static char dividerVersion[32] = "qdivSequential";

static char qmultSequential[4096] = "\
module qmultSequential #(\n\
	//Parameterized values\n\
	parameter Q = 15,\n\
	parameter N = 32\n\
	)\n\
	(\n\
	input 	[N-1:0] i_multiplicand,\n\
	input 	[N-1:0]	i_multiplier,\n\
	input 	i_start,\n\
	input 	i_clk,\n\
	output 	[N-1:0] o_result_out,\n\
	output 	o_complete,\n\
	output	o_overflow\n\
	);\n\
\n\
	reg [2*N-2:0]	reg_working_result;		//	a place to accumulate our result\n\
	reg [2*N-2:0]	reg_multiplier_temp;		//	a working copy of the multiplier\n\
	reg [N-1:0]	reg_multiplicand_temp;		//	a working copy of the umultiplicand\n\
	reg [N-1:0]	reg_result_temp;\n\
	wire [N-1:0] 	i_multiplicand_unsigned;\n\
	wire [N-1:0] 	i_multiplier_unsigned;\n\
	reg [N-1:0] 	reg_count; 			//	This is obviously a lot bigger than it needs to be, as we only need \n\
							//		count to N, but computing that number of bits requires a \n\
							//		logarithm (base 2), and I don't know how to do that in a \n\
							//		way that will work for every possibility						 \n\
    reg reg_working;\n\
	reg	reg_done;				//	Computation completed flag\n\
	reg	reg_sign;				//	The result's sign bit\n\
	reg	reg_overflow;				//	Overflow flag\n\
\n\
    initial reg_working = 1'b0;		//	Initial state is to not be doing anything\n\
	initial reg_done = 1'b0;		//	Initial state is to not be doing anything\n\
	initial reg_overflow = 1'b0;		//		And there should be no woverflow present\n\
	initial reg_sign = 1'b0;		//		And the sign should be positive\n\
\n\
	assign o_result_out[N-1:0] = reg_result_temp[N-1:0];\n\
	//reg_working_result[N-2+Q:Q];	//	The multiplication results\n\
	assign i_multiplicand_unsigned = ~i_multiplicand[N-1:0] + 1;\n\
	assign i_multiplier_unsigned = ~i_multiplier[N-1:0] + 1;\n\
	assign o_complete = reg_done;					//	Done flag\n\
	assign o_overflow = reg_overflow;				//	Overflow flag\n\
\n\
	always @( posedge i_clk ) begin\n\
		if( !reg_working && i_start ) begin				//	This is our startup condition\n\
			reg_done <= 1'b0;				//	We're not done\n\
			reg_working <= 1'b1;		//	Initial state is to not be doing anything			\n\
			reg_count <= 0;					//	Reset the count\n\
			reg_working_result <= 0;			//	Clear out the result register\n\
			reg_overflow <= 1'b0;				//	Clear the overflow register\n\
\n\
			reg_sign <= i_multiplicand[N-1] ^ i_multiplier[N-1];		//	Set the sign bit\n\
\n\
			if (i_multiplicand[N-1] == 1) \n\
				reg_multiplicand_temp <= i_multiplicand_unsigned[N-1:0]; //~i_multiplicand[N-1:0] + 1;	//	Left-align the dividend in its working register\n\
			else\n\
				reg_multiplicand_temp <= i_multiplicand[N-1:0];		//	Left-align the dividend in its working register\n\
\n\
			if (i_multiplier[N-1] == 1) \n\
				reg_multiplier_temp <= i_multiplier_unsigned[N-1:0]; //~i_multiplier[N-1:0] + 1;	//	Left-align the divisor into its working register\n\
			else\n\
				reg_multiplier_temp <= i_multiplier[N-1:0];		//	Left-align the divisor into its working register \n\
\n\
		end \n\
		else if (reg_working) begin\n\
			if (reg_multiplicand_temp[reg_count] == 1'b1)				//	if the appropriate multiplicand bit is 1\n\
				reg_working_result <= reg_working_result + reg_multiplier_temp;	//	then add the temp multiplier\n\
\n\
			reg_multiplier_temp <= reg_multiplier_temp << 1;			//	Do a left-shift on the multiplier\n\
			reg_count <= reg_count + 1;						//	Increment the count\n\
\n\
			//stop condition\n\
			if(reg_count == N) begin\n\
				reg_done <= 1'b1;						//	If we're done, it's time to tell the calling process\n\
				reg_working <= 1'b0;		//	Initial state is to not be doing anything\n\
\n\
				if (reg_sign == 1)\n\
					reg_result_temp[N-1:0] <= ~reg_working_result[N-2+Q:Q] + 1;\n\
				else\n\
					reg_result_temp[N-1:0] <= reg_working_result[N-2+Q:Q];\n\
\n\
				if (reg_working_result[2*N-2:N-1+Q] > 0)			// Check for an overflow\n\
					reg_overflow <= 1'b1;\n\
												//	Increment the count\n\
			end\n\
		end\n\
	end\n\
    endmodule\n";

static char qdivSequential[8192] = "\
module qdivSequential #(\n\
	//Parameterized values\n\
	parameter Q = 15,\n\
	parameter N = 32\n\
	)\n\
	(\n\
	input 	[N-1:0] i_dividend,\n\
	input 	[N-1:0] i_divisor,\n\
	input 	i_start,\n\
	input 	i_clk,\n\
	output 	[N-1:0] o_quotient_out,\n\
	output 	o_complete,\n\
	output	o_overflow\n\
	);\n\
\n\
	wire [N-1:0] 	reg_dividend_unsigned;	//\n\
	wire [N-1:0] 	reg_divisor_unsigned;	//\n\
	wire [N-1:0] 	reg_quotient_signed;	//	\n\
\n\
	reg [2*N+Q-3:0]	reg_working_quotient;	//	Our working copy of the quotient\n\
	reg [N-1:0] 	reg_quotient_unsigned;	//	Final quotient\n\
	reg [N-1:0] 	reg_quotient;		//	Final quotient\n\
	reg [N-2+Q:0] 	reg_working_dividend;	//	Working copy of the dividend\n\
	reg [2*N+Q-3:0]	reg_working_divisor;	// Working copy of the divisor\n\
\n\
	reg [N-1:0] reg_count; 		//	This is obviously a lot bigger than it needs to be, as we only need \n\
					           //		count to N-1+Q but, computing that number of bits requires a \n\
					           //		logarithm (base 2), and I don't know how to do that in a \n\
					           //		way that will work for everyone\n\
\n\
	reg reg_working;									 \n\
	reg reg_done;		//	Computation completed flag\n\
	reg	reg_sign;		//	The quotient's sign bit\n\
	reg	reg_overflow;		//	Overflow flag\n\
\n\
	initial reg_done = 1'b0;	//	Initial state is to not be doing anything\n\
	initial reg_working = 1'b0;		//	Initial state is to not be doing anything\n\
	initial reg_overflow = 1'b0;	//	And there should be no overflow present\n\
	initial reg_sign = 1'b0;	//	And the sign should be positive\n\
\n\
	initial reg_working_quotient = 0;	\n\
	initial reg_quotient = 0;				\n\
	initial reg_working_dividend = 0;	\n\
	initial reg_working_divisor = 0;		\n\
 	initial reg_count = 0;\n\
\n\
	assign reg_dividend_unsigned = ~i_dividend + 1;\n\
	assign reg_divisor_unsigned = ~i_divisor + 1;\n\
	assign o_quotient_out[N-1:0] = reg_quotient[N-1:0];	//	The division results\n\
	assign o_complete = reg_done;\n\
	assign o_overflow = reg_overflow;\n\
\n\
	always @( posedge i_clk ) begin\n\
		if( !reg_working && i_start ) begin			//	This is our startup condition\n\
			//  Need to check for a divide by zero right here, I think....\n\
			reg_done <= 1'b0;			     //	We're not done\n\
			reg_working <= 1'b1;		//				\n\
			reg_count <= N+Q-1;			     //	Set the count\n\
			reg_working_quotient <= 0;		//	Clear out the quotient register\n\
			reg_working_dividend <= 0;		//	Clear out the dividend register \n\
			reg_working_divisor <= 0;		//	Clear out the divisor register \n\
			reg_overflow <= 1'b0;			//	Clear the overflow register\n\
\n\
			if (i_dividend[N-1] == 1) \n\
				reg_working_dividend[N+Q-2:Q] <= reg_dividend_unsigned[N-2:0];		//	Left-align the dividend in its working register\n\
			else\n\
				reg_working_dividend[N+Q-2:Q] <= i_dividend[N-2:0];		           //	Left-align the dividend in its working register\n\
\n\
			if (i_divisor[N-1] == 1) \n\
				reg_working_divisor[2*N+Q-3:N+Q-1] <= reg_divisor_unsigned[N-2:0];		//	Left-align the divisor into its working register\n\
			else\n\
				reg_working_divisor[2*N+Q-3:N+Q-1] <= i_divisor[N-2:0];		           //	Left-align the divisor into its working register \n\
\n\
			reg_sign <= i_dividend[N-1] ^ i_divisor[N-1];		//	Set the sign bit\n\
		end \n\
		else if(reg_working) begin\n\
			reg_working_divisor = reg_working_divisor >> 1;	//	Right shift the divisor (that is, divide it by two - aka reduce the divisor)\n\
			reg_count = reg_count - 1;				//	Decrement the count\n\
\n\
			//	If the dividend is greater than the divisor\n\
			if(reg_working_dividend >= reg_working_divisor) begin\n\
				reg_working_quotient[reg_count] = 1'b1;				//	Set the quotient bit\n\
				reg_working_dividend = reg_working_dividend - reg_working_divisor;	//	and subtract the divisor from the dividend\n\
			end\n\
\n\
			//stop condition\n\
			if(reg_count == 0) begin\n\
				reg_done = 1'b1;				                        //	If we're done, it's time to tell the calling process\n\
				reg_working = 1'b0;		                                 //	\n\
				reg_quotient_unsigned = reg_working_quotient[N-1:0];	//	Move in our working copy to the outside world\n\
\n\
				if (reg_sign == 1)\n\
					reg_quotient = ~reg_quotient_unsigned + 1;\n\
				else\n\
					reg_quotient = reg_working_quotient[N-1:0];\n\
\n\
				if (reg_working_quotient[2*N+Q-3:N]>0)\n\
					reg_overflow = 1'b1;\n\
			end\n\
		end\n\
	end\n\
endmodule\n";


typedef struct multChainTag multChain;

struct multChainTag {
	char *operandName;
	multChain *next;
}; 

/* Get the fraction of a power and reverse it -- FIXME check what happens with Irrational numbers */
int
irPassRTLGetFraction(float floatingPointNumber)
{
	int dec;
	float fp, reverse;

	floatingPointNumber = fabsf(floatingPointNumber);
	dec = (int) (floorf(floatingPointNumber));
	fp = floatingPointNumber - dec;

	if (fp > 0) {
		reverse = 1.0 / fp;
	} else {
		reverse = 1.0;
	}
	

	return (int) reverse;
}

int 
irPassRTLCalculateLCM(int* numbers, int len) {
    int lcm = 1;
    int divisor = 2;
    
	while (1) {
        int cnt = 0;
        int divisible = 0;

        for (int i = 0; i < len; i++) {         
			/**
             * lcm (n1,n2,... 0)=0.For negative number we convert into
             * positive and calculate lcm.
             */
            if (numbers[i] == 0) {
                return 0;
            } else if (numbers[i] < 0) {
                numbers[i] = numbers[i] * (-1);
            }

            if (numbers[i] == 1) {
                cnt++;
            }
            /**
             * divide numbers by devisor if complete division i.e. without
             * remainder then replace number with quotient; used for find
             * next factor
             */
            if (numbers[i] % divisor == 0) {
                divisible = 1;
                numbers[i] = numbers[i] / divisor;
            }
        }
        /**
         * If divisor able to completely divide any number from array
         * multiply with lcm and store into lcm and continue to same divisor
         * for next factor finding. else increment divisor
         */
        if (divisible) {
            lcm = lcm * divisor;
        } else {
            divisor++;
        }
        /**
         * Check if all numbers is 1 indicate we found all factors and
         * terminate while loop.
         */
        if (cnt ==len) {
            return lcm;
        }
    }
}

void
irPassRTLSearchAndCreateArgList(State *  N, IrNode *  root, IrNodeType expectedType, char **argList, int argumentIndex)
{
	if (root == NULL)
	{
		return;
	}

	if (root->irRightChild == NULL && root->irLeftChild == NULL
		&& root->type == expectedType)
	{
		argList[argumentIndex] = (char *) malloc((strlen(root->tokenString) + 1) * sizeof(char));
		strcpy(argList[argumentIndex], root->tokenString);
		//flexprint(N->Fe, N->Fm, N->Fprtl, "%s", root->tokenString);
		
		return;
	}

	irPassRTLSearchAndCreateArgList(N, root->irLeftChild, expectedType, argList, argumentIndex);
	
	return;
}


/* FIXME check if mallocs and copies fail */
void irPassRTLAddMultChain(multChain **multChainHead, char *parameterName, int power) 
{
	multChain *tmpChainNode=NULL;
	int i;

	if (*multChainHead == NULL) {
		*multChainHead = (multChain *) malloc(sizeof(multChain));
		(*multChainHead)->operandName = NULL;
		(*multChainHead)->next = NULL;
	}

	tmpChainNode = *multChainHead;

	/* Go to the end of the list */
	while (tmpChainNode->next != NULL) {
		tmpChainNode = tmpChainNode->next;
	}

	for (i=0; i<power; i++) {
		if (tmpChainNode->operandName != NULL) { /* If it is empty then it should be filled */
			tmpChainNode->next = (multChain *) malloc(sizeof(multChain));
			tmpChainNode = tmpChainNode->next;
		}

		tmpChainNode->operandName = (char *) malloc((strlen(parameterName)+1) * sizeof(char)); /* +1 for \0 at the end */
		tmpChainNode->operandName[0] = '\0';

		strcpy(tmpChainNode->operandName, parameterName);
		tmpChainNode->next = NULL;
	}
}

void
irPassRTLProcessInvariantList(State *  N)
{
	Invariant *	targetInvariant = N->invariantList;

	/*
	 *	Check if the file called is simply an include.nt
	 */
	if (N->invariantList == NULL)
	{
		flexprint(N->Fe, N->Fm, N->Fprtl, "/*\n *\tInvariantList is NULL\n */\n");
		return;
	}
	
	/* Constraints list removed */
	IrNode *	parameterListXSeq = targetInvariant->parameterList->irParent->irLeftChild;

	char ** argumentsList;
		
	int index = 0;

	int targetKernel = N->targetParamLocatedKernel;

    /* FIXME add checks - to CBackend as well if current assumptions are met */

	flexprint(N->Fe, N->Fm, N->Fprtl, "/*\n *\tGenerated .v file from Newton\n */\n\n");
	flexprint(N->Fe, N->Fm, N->Fprtl, "`timescale 1 ns/ 100 ps\n\n");

	flexprint(N->Fe, N->Fm, N->Fprtl, "%s\n", qmultSequential);
	flexprint(N->Fe, N->Fm, N->Fprtl, "\n");
	flexprint(N->Fe, N->Fm, N->Fprtl, "%s\n", qdivSequential);
	flexprint(N->Fe, N->Fm, N->Fprtl, "\n");

	if (targetInvariant != NULL)
	{
		argumentsList = (char **) malloc(targetInvariant->dimensionalMatrixColumnCount * sizeof(char *));

		/*
		*	Print calculation module -- FIXME more clear support of various architectures
		*/
		flexprint(N->Fe, N->Fm, N->Fprtl, "module %sSerial", targetInvariant->identifier);

		flexprint(N->Fe, N->Fm, N->Fprtl, " #(\n\t//Parameterized values\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tparameter Q = %d,\n", Q_PARAMETER);
        flexprint(N->Fe, N->Fm, N->Fprtl, "\tparameter N = %d", N_PARAMETER);
        flexprint(N->Fe, N->Fm, N->Fprtl, "\n\t)\n\t(\n", N_PARAMETER);
#ifndef ICE40
        flexprint(N->Fe, N->Fm, N->Fprtl, "\tinput\ti_clk,\n");
#endif
		while (parameterListXSeq != NULL) /* Create a list of function parameters */
		{
			irPassRTLSearchAndCreateArgList(N, parameterListXSeq->irLeftChild, kNewtonIrNodeType_Tidentifier, argumentsList, index);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\tinput\t[N-1:0] %s_sig,\n", argumentsList[index]);			
			
			parameterListXSeq = parameterListXSeq->irRightChild;
			index++;
		}
		
		/*
		 *	Declare
		 */
        for (int piIndex = 0; piIndex < targetInvariant->kernelColumnCount-1; piIndex++) 
		{
			flexprint(N->Fe, N->Fm, N->Fprtl, "\toutput\t[N-1:0] pi_%d_calcSig,\n", piIndex);
		}
		/* Avoid printing a comma for the last Pi calculation signal */
		flexprint(N->Fe, N->Fm, N->Fprtl, "\toutput\t[N-1:0] pi_%d_calcSig\n", (targetInvariant->kernelColumnCount-1));
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t);\n\n");

		flexprint(N->Fe, N->Fm, N->Fprtl, "\treg\ti_st;\n");

		for (int piIndex = 0; piIndex < targetInvariant->kernelColumnCount; piIndex++) 
		{
			flexprint(N->Fe, N->Fm, N->Fprtl, "\treg\t[N-1:0] pi_%d_calcReg;\n", piIndex);
		}
		flexprint(N->Fe, N->Fm, N->Fprtl, "\twire\toc_Pi;\n");
#ifdef ICE40
        flexprint(N->Fe, N->Fm, N->Fprtl, "\twire\ti_clk;\n\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t/* \n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t *\tCreates a 48MHz clock signal from\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t *\tinternal oscillator of the iCE40\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t */\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tSB_HFOSC OSCInst0 (\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.CLKHFPU(1'b1),\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.CLKHFEN(1'b1),\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.CLKHF(i_clk)\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t);\n\n");
#endif	
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tinitial i_st = 1'b0;\n\n");
		/*
		 *	We construct a temporary array to re-locate the positions of the permuted parameters
		 */
		int *tmpPosition = (int *)calloc(targetInvariant->dimensionalMatrixColumnCount, sizeof(int));
		int *fractionValues = (int *)calloc(targetInvariant->dimensionalMatrixColumnCount, sizeof(int));
		int divisorMultiplications=0, dividendMultiplications=0, fractionsLCM=1, integerPower;
		multChain *dividendMultChainHead=NULL, *divisorMultChainHead=NULL, *tmpChainNode;

		for (int j = 0; j < targetInvariant->dimensionalMatrixColumnCount; j++)
		{
			tmpPosition[targetInvariant->permutedIndexArrayPointer[targetKernel * targetInvariant->dimensionalMatrixColumnCount + j]] = j;
		}

		flexprint(N->Fe, N->Fm, N->Fprtl, "\t/* ----- Original Pi values were ----- */\n");

		/* FIXME!!! - Need to free the nodes of the list when traversing!!! */
		/*
		 *	One pass to determine the number of required multiplications in divisor and dividend.
		 * 	Also create a chain of multiplications that need to be performed. Then deploy RTL based on that list.
		 */			
		for (int col = 0; col < targetInvariant->kernelColumnCount; col++)
		{
			index = 0;
			/* targetInvariant->kernelColumnCount equals to the number of Pis in the designated Pi group */
			flexprint(N->Fe, N->Fm, N->Fprtl, "\t/* ----- Pi %d ----- \n", col);

			for (int row = 0; row < targetInvariant->dimensionalMatrixColumnCount; row++)
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t * Param %d: %f \n", index, targetInvariant->nullSpace[targetKernel][tmpPosition[row]][col]);
				if (targetInvariant->nullSpace[targetKernel][tmpPosition[row]][col] != 0) 
				{
					fractionValues[index] = irPassRTLGetFraction(targetInvariant->nullSpace[targetKernel][tmpPosition[row]][col]);
				} 
				else 
				{
					fractionValues[index] = 1; /* Won't affect LCM calculation */
				}
				index++;
			}
		
			fractionsLCM = irPassRTLCalculateLCM(fractionValues, targetInvariant->dimensionalMatrixColumnCount);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\t * fractionsLCM %d \n", fractionsLCM);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\t */\n\n");
			
			flexprint(N->Fe, N->Fm, N->Fprtl, "\t/* ----- Calculations for Pi %d ----- */\n", col);

			flexprint(N->Fe, N->Fm, N->Fprtl, "\twire [N-1:0] division_res_Pi_%d;\n", col);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\twire [N-1:0] dividend_Pi_%d;\n", col);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\twire [N-1:0] divisor_Pi_%d;\n\n", col);
		
			flexprint(N->Fe, N->Fm, N->Fprtl, "\twire oc_Pi_%d;\n", col);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\twire of_Pi_%d;\n", col);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\twire i_st_ratio_Pi_%d;\n", col);	

			divisorMultiplications = 0;
			dividendMultiplications = 0;

			/* Clear dividendMultChainHead */
			while (dividendMultChainHead != NULL) 
			{
				tmpChainNode = dividendMultChainHead;
				dividendMultChainHead = dividendMultChainHead->next;
				free(tmpChainNode);
			}

			/* Clear divisorMultChainHead */
			while (divisorMultChainHead != NULL) 
			{
				tmpChainNode = divisorMultChainHead;
				divisorMultChainHead = divisorMultChainHead->next;
				free(tmpChainNode);
			}

			for (int row = 0; row < targetInvariant->dimensionalMatrixColumnCount; row++)
			{
				if (targetInvariant->nullSpace[targetKernel][tmpPosition[row]][col] != 0) 
				{
					integerPower = (int) (fractionsLCM * targetInvariant->nullSpace[targetKernel][tmpPosition[row]][col]);

					if (integerPower >= 0) 
					{ /* Can there be a zero? */
						dividendMultiplications += integerPower;
						irPassRTLAddMultChain (&dividendMultChainHead, argumentsList[row], integerPower);
					} 
					else 
					{
						divisorMultiplications += -integerPower; /* FIXME what happens in case of fraction? */
						irPassRTLAddMultChain (&divisorMultChainHead, argumentsList[row], -integerPower);
					}
				}
			}
		
			/* 	If there is positive number of multiplications either on nominator 					*/
			/*	or denominator, then they are one less multiplication than the sum of powers		*/
			/*	If there are no multiplications to perform either in the nominator or denominator,	*/
			/*	then dividendMultiplications and divisorMultiplications are set to -1.				*/			
			dividendMultiplications--;
			
			divisorMultiplications--;
						
			/*
			 *	Print declarations of intermediate dividend multiplication wires
			 */
			if (dividendMultiplications <= 0) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\treg oc_dividend_Pi_%d;\n", col);
				
				flexprint(N->Fe, N->Fm, N->Fprtl, "\tinitial oc_dividend_Pi_%d = 1'b1;\n", col);
			} 
			else if (dividendMultiplications == 1) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\twire oc_dividend_Pi_%d;\n", col);
			} 
			else if (dividendMultiplications > 1) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\twire oc_dividend_Pi_%d;\n", col);

				for (index=1; index < dividendMultiplications; index++) 
				{
					flexprint(N->Fe, N->Fm, N->Fprtl, "\twire [N-1:0]mult_res_dividend_inter_Pi_%d_%d;\n", col, index);
					flexprint(N->Fe, N->Fm, N->Fprtl, "\twire oc_dividend_Pi_%d_%d;\n", col, index);
				}
			} 
			else 
			{
				/* FIXME if negative */	
			}

			/*
			 *	Print declarations of intermediate divisor multiplication wires
			 */
			if (divisorMultiplications <= 0) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\treg oc_divisor_Pi_%d;\n", col);

				flexprint(N->Fe, N->Fm, N->Fprtl, "\n\tinitial oc_divisor_Pi_%d = 1'b1;\n", col);
			} 
			else if (divisorMultiplications == 1) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\twire oc_divisor_Pi_%d;\n", col);
			} 
			else if (divisorMultiplications > 1) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\twire oc_divisor_Pi_%d;\n", col);

				for (index=1; index < divisorMultiplications; index++) 
				{
					flexprint(N->Fe, N->Fm, N->Fprtl, "\twire [N-1:0] mult_res_divisor_inter_Pi_%d_%d;\n", col, index);	
					flexprint(N->Fe, N->Fm, N->Fprtl, "\twire oc_divisor_Pi_%d_%d;\n", col, index);
				}
			} 
			else 
			{
				/* FIXME if negative */	
			}
			
			flexprint(N->Fe, N->Fm, N->Fprtl, "\n\t/* ----- Dividend ----- */\n");

			if (dividendMultiplications < 0) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\tassign dividend_Pi_%d = N'b1;\n", col);	
			} 
			else if (dividendMultiplications == 0) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\tassign dividend_Pi_%d = %s_sig;\n", col, dividendMultChainHead->operandName);	
			} 
			else if (dividendMultiplications == 1 ) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t%s mul_inst_dividend_Pi_%d (\n\t\t.i_multiplicand(%s_sig),\n", 
					multiplierVersion, col, dividendMultChainHead->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_multiplier(%s_sig),\n", dividendMultChainHead->next->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t\t.i_start(i_st),\
					\n\t\t.i_clk(i_clk),\
					\n\t\t.o_result_out(dividend_Pi_%d),\
					\n\t\t.o_complete(oc_dividend_Pi_%d),\
					\n\t\t.o_overflow(of_Pi_%d));\n\n", col, col, col);
			} 
			else 
			{
				index = 1;

				/* First multiplication with two straightforward operands */
				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t%s mul_inst_dividend_Pi_%d_%d (\n\t\t.i_multiplicand(%s_sig),\n",multiplierVersion, col, index, dividendMultChainHead->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_multiplier(%s_sig),\n", dividendMultChainHead->next->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_start(i_st),\n");
				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t\t.i_clk(i_clk),\
					\n\t\t.o_result_out(mult_res_dividend_inter_Pi_%d_%d),\
					\n\t\t.o_complete(oc_dividend_Pi_%d_%d),\
					\n\t\t.o_overflow(of_Pi_%d)\n\t);\n\n", col, index, col, index, col);

				dividendMultChainHead = dividendMultChainHead->next->next;
				index++;

				while (dividendMultChainHead->next != NULL) 
				{
					flexprint(N->Fe, N->Fm, N->Fprtl, 
						"\t%s mul_inst_dividend_Pi_%d_%d (\n\t\t.i_multiplicand(mult_res_dividend_inter_Pi_%d_%d),\n", multiplierVersion, col, index, col, index-1);
					flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_multiplier(%s_sig),\n", dividendMultChainHead->operandName);
					flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_start(oc_dividend_Pi_%d_%d),\n", col, index-1);
					flexprint(N->Fe, N->Fm, N->Fprtl, 
						"\t\t.i_clk(i_clk),\
						\n\t\t.o_result_out(mult_res_dividend_inter_Pi_%d_%d),\
						\n\t\t.o_complete(oc_dividend_Pi_%d_%d),\
						\n\t\t.o_overflow(of_Pi_%d)\n\t);\n\n", col, index, col, index, col);
				
					dividendMultChainHead = dividendMultChainHead->next;
					index++;
				}

				flexprint(N->Fe, N->Fm, N->Fprtl, "\t%s mul_inst_dividend_Pi_%d_%d (\n\t\t.i_multiplicand(mult_res_dividend_inter_Pi_%d_%d),\n", multiplierVersion, col, index, col, index-1);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_multiplier(%s_sig),\n", dividendMultChainHead->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_start(oc_dividend_Pi_%d_%d),\n", col, index-1);
				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t\t.i_clk(i_clk),\
					\n\t\t.o_result_out(dividend_Pi_%d),\
					\n\t\t.o_complete(oc_dividend_Pi_%d),\
					\n\t\t.o_overflow(of_Pi_%d)\n\t);\n\n", col, col, col);
			}

			flexprint(N->Fe, N->Fm, N->Fprtl, "\n\t/* ----- Divisor ----- */\n");
			if (divisorMultiplications < 0) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\tassign divisor_Pi_%d = N'b1;\n", col);	
			}
			else if (divisorMultiplications == 0) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "\tassign divisor_Pi_%d = %s_sig;\n", col, divisorMultChainHead->operandName);	
			} 
			else if (divisorMultiplications == 1) 
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t%s mul_inst_divisor_Pi_%d (\n\t\t.i_multiplicand(%s_sig),\n", multiplierVersion, col, divisorMultChainHead->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_multiplier(%s_sig),\n", divisorMultChainHead->next->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t\t.i_start(i_st),\
					\n\t\t.i_clk(i_clk),\
					\n\t\t.o_result_out(divisor_Pi_%d),\
					\n\t\t.o_complete(oc_divisor_Pi_%d),\
					\n\t\t.o_overflow(of_Pi_%d)\n\t);\n\n", col, col, col);
			} 
			else 
			{
				index = 1;

				/* First multiplication with two straightforward operands */
				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t%s mul_inst_divisor_Pi_%d_%d (\n\t\t.i_multiplicand(%s_sig),\n", multiplierVersion, col, index, divisorMultChainHead->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_multiplier(%s_sig),\n", divisorMultChainHead->next->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_start(i_st),\n");
				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t\t.i_clk(i_clk),\
					\n\t\t.o_result_out(mult_res_divisor_inter_Pi_%d_%d),\
					\n\t\t.o_complete(oc_divisor_Pi_%d_%d),\
					\n\t\t.o_overflow(of_Pi_%d)\n\t);\n\n", col, index, col, index, col);

				divisorMultChainHead = divisorMultChainHead->next->next;
				index++;

				while (divisorMultChainHead->next != NULL) 
				{
					flexprint(N->Fe, N->Fm, N->Fprtl, 
						"\t%s mul_inst_divisor_Pi_%d_%d (\n\t\t.i_multiplicand(mult_res_divisor_inter_Pi_%d_%d),\n", multiplierVersion, col, index, col, index-1);
					flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_multiplier(%s_sig),\n", divisorMultChainHead->operandName);
					flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_start(oc_divisor_Pi_%d_%d),\n", col, index-1);
					flexprint(N->Fe, N->Fm, N->Fprtl, 
						"\t\t.i_clk(i_clk),\
						\n\t\t.o_result_out(mult_res_divisor_inter_Pi_%d_%d),\
						\n\t\t.o_complete(oc_divisor_Pi_%d_%d),\
						\n\t\t.o_overflow(of_Pi_%d)\n\t);\n\n", col, index, col, index, col);
				
					divisorMultChainHead = divisorMultChainHead->next;
					index++;
				}

				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t%s mul_inst_divisor_Pi_%d_%d (\n\t\t.i_multiplicand(mult_res_divisor_inter_Pi_%d_%d),\n", multiplierVersion, col, index, col, index-1);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_multiplier(%s_sig),\n", divisorMultChainHead->operandName);
				flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_start(oc_divisor_Pi_%d_%d),\n", col, index-1);
				flexprint(N->Fe, N->Fm, N->Fprtl, 
					"\t\t.i_clk(i_clk),\
					\n\t\t.o_result_out(divisor_Pi_%d),\
					\n\t\t.o_complete(oc_divisor_Pi_%d),\
					\n\t\t.o_overflow(of_Pi_%d)\n\t);\n\n", col, col, col);
			}

			flexprint(N->Fe, N->Fm, N->Fprtl, "\n\t/* ----- Division enabling ----- */\n");
			flexprint(N->Fe, N->Fm, N->Fprtl, "\tassign i_st_ratio_Pi_%d = oc_dividend_Pi_%d & oc_divisor_Pi_%d;\n\n", col, col, col);

			flexprint(N->Fe, N->Fm, N->Fprtl, "\t/* ----- Division ----- */\n");
			flexprint(N->Fe, N->Fm, N->Fprtl, 
				"\t%s qdiv_inst_Pi_%d (\
				\n\t\t.i_dividend(dividend_Pi_%d),\
				\n\t\t.i_divisor(divisor_Pi_%d),\
				\n\t\t.i_start(i_st_ratio_Pi_%d),\
				\n\t\t.i_clk(i_clk),\
				\n\t\t.o_quotient_out(division_res_Pi_%d),\
				\n\t\t.o_complete(oc_Pi_%d),\
				\n\t\t.o_overflow(of_Pi_%d)\n\t);\n\n", dividerVersion, col, col, col, col, col, col, col);
		}

		for (int piIndex = 0; piIndex < targetInvariant->kernelColumnCount; piIndex++) 
		{
			flexprint(N->Fe, N->Fm, N->Fprtl, "\tassign pi_%d_calcSig = pi_%d_calcReg;\n", piIndex, piIndex);
		}
		
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tassign oc_Pi = ");
		for (int piIndex = 0; piIndex < targetInvariant->kernelColumnCount-1; piIndex++) 
		{
			flexprint(N->Fe, N->Fm, N->Fprtl, "oc_Pi_%d & ", piIndex);
		}
		flexprint(N->Fe, N->Fm, N->Fprtl, "oc_Pi_%d;\n", targetInvariant->kernelColumnCount-1);
		flexprint(N->Fe, N->Fm, N->Fprtl, "\n");

		flexprint(N->Fe, N->Fm, N->Fprtl, "\talways @( posedge i_clk )\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tbegin\n\n");
	   
       	flexprint(N->Fe, N->Fm, N->Fprtl, "\t\tif (~i_st) begin\n");
        flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t\ti_st = 1'b1;\n");
       	flexprint(N->Fe, N->Fm, N->Fprtl, "\t\tend\n");
       	flexprint(N->Fe, N->Fm, N->Fprtl, "\t\telse begin\n");
        flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t\tif (oc_Pi) begin\n");
    	
		for (int piIndex = 0; piIndex < targetInvariant->kernelColumnCount; piIndex++) 
		{
			flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t\t\tpi_%d_calcReg = division_res_Pi_%d;\n", piIndex, piIndex);
		}

    	flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t\t\ti_st = 1'b0;\n");
        flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t\tend\n");
       	flexprint(N->Fe, N->Fm, N->Fprtl, "\t\tend\n");	   
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tend\n");

		flexprint(N->Fe, N->Fm, N->Fprtl, "\t/* the \"macro\" to dump signals */\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "`ifdef COCOTB_SIM\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tinitial begin\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t$dumpfile (\"%sSerial.vcd\");\n", targetInvariant->identifier);
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t$dumpvars (0, %sSerial);\n", targetInvariant->identifier);
	  	flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t#1;\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tend\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "`endif\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "endmodule\n\n");

		/************ Print Top module including LFSR *************/
		flexprint(N->Fe, N->Fm, N->Fprtl, "module %sTopLFSR #(parameter N = 32, W = 24, V = 18, g_type = 0, u_type = 1)\n",
				targetInvariant->identifier);
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t(\n");		
		/* Ouputs */
        for (int piIndex = 0; piIndex < targetInvariant->kernelColumnCount-1; piIndex++) 
		{
			flexprint(N->Fe, N->Fm, N->Fprtl, "\t\toutput\t pi_%d_calcSig,\n", piIndex);
		}
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\toutput\t pi_%d_calcSig\n", targetInvariant->kernelColumnCount-1);
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t);\n\n");

		flexprint(N->Fe, N->Fm, N->Fprtl, "\twire\tclk12;\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\treg\tENCLKHF\t= 1'b1;	// Plock enable\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\treg\tCLKHF_POWERUP\t= 1'b1;	// Power up the HFOSC circuit\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\treg\ti_rst;\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tinitial i_rst = 1'b0;\n\n");
		
		parameterListXSeq = targetInvariant->parameterList->irParent->irLeftChild;
		while (parameterListXSeq != NULL) /* Create a list of function parameters */
		{
			irPassRTLSearchAndCreateArgList(N, parameterListXSeq->irLeftChild, kNewtonIrNodeType_Tidentifier, argumentsList, index);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\twire\t[N-1:0] %s_sigWire;\n", argumentsList[index]);			
			
			parameterListXSeq = parameterListXSeq->irRightChild;
			index++;
		}
		/* Ouputs */
        for (int piIndex = 0; piIndex < targetInvariant->kernelColumnCount; piIndex++) 
		{
			flexprint(N->Fe, N->Fm, N->Fprtl, "\twire\t[N-1:0] pi_%d_calcSigWire;\n", piIndex);
		}

		flexprint(N->Fe, N->Fm, N->Fprtl, "\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\twire [W-1 : 0] 	randG_out;\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\twire [W-1 : 0] 	randU_out;\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\n");
		
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t/* \n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t *\tCreates a 12MHz clock signal from\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t *\tinternal oscillator of the iCE40\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t */\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tSB_HFOSC #(.CLKHF_DIV(\"0b10\")) OSCInst0 (\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.CLKHFEN(ENCLKHF),\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.CLKHFPU(CLKHF_POWERUP),\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.CLKHF(clk12)\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t);\n\n");

		flexprint(N->Fe, N->Fm, N->Fprtl, "\t%sSerial %sRTL (\n", targetInvariant->identifier, targetInvariant->identifier);
		parameterListXSeq = targetInvariant->parameterList->irParent->irLeftChild;
		while (parameterListXSeq != NULL) /* Create a list of function parameters */
		{
			irPassRTLSearchAndCreateArgList(N, parameterListXSeq->irLeftChild, kNewtonIrNodeType_Tidentifier, argumentsList, index);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.%s_sig(%s_sigWire),\n", argumentsList[index], argumentsList[index]);			
			
			parameterListXSeq = parameterListXSeq->irRightChild;
			index++;
		}
		
		/* Ouputs */
        for (int piIndex = 0; piIndex < targetInvariant->kernelColumnCount; piIndex++) 
		{
			flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.pi_%d_calcSig(pi_%d_calcSigWire),\n", piIndex, piIndex);
		}
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.i_clk(clk12)\n", (targetInvariant->kernelColumnCount-1));
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t);\n\n");

		flexprint(N->Fe, N->Fm, N->Fprtl, "\tLFSR_Plus LFSR_PlusInt (\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.g_noise_out(randG_out),\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.u_noise_out(randU_out),\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.clk(clk12),\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.n_reset(i_rst),\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t.enable(ENCLKHF)\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\t);\n\n");

		/* Ouputs */
        for (int piIndex = 0; piIndex < targetInvariant->kernelColumnCount; piIndex++) 
		{
			flexprint(N->Fe, N->Fm, N->Fprtl, "\tassign\t pi_%d_calcSig = ", piIndex);

			for (int i=31; i>0; i--)
			{
				flexprint(N->Fe, N->Fm, N->Fprtl, "pi_%d_calcSigWire[%d] | ", piIndex, i);
				if (i % 8 == 0) 
				{
					flexprint(N->Fe, N->Fm, N->Fprtl, "\n\t\t");
				} 
			}
			flexprint(N->Fe, N->Fm, N->Fprtl, "pi_%d_calcSigWire[0];\n\n", piIndex);
		}

		parameterListXSeq = targetInvariant->parameterList->irParent->irLeftChild;
		while (parameterListXSeq != NULL) /* Create a list of function parameters */
		{
			irPassRTLSearchAndCreateArgList(N, parameterListXSeq->irLeftChild, kNewtonIrNodeType_Tidentifier, argumentsList, index);
			flexprint(N->Fe, N->Fm, N->Fprtl, "\tassign %s_sigWire = randG_out;\n\n", argumentsList[index]);			
			
			parameterListXSeq = parameterListXSeq->irRightChild;
			index++;
		}
		flexprint(N->Fe, N->Fm, N->Fprtl, "\n");

		flexprint(N->Fe, N->Fm, N->Fprtl, "\talways @( posedge clk12 )\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "\tbegin\n\n");
	   
       	flexprint(N->Fe, N->Fm, N->Fprtl, "\t\tif (~i_rst) begin\n");
        flexprint(N->Fe, N->Fm, N->Fprtl, "\t\t\ti_rst <= 1'b1;\n");
       	flexprint(N->Fe, N->Fm, N->Fprtl, "\t\tend\n");
       	flexprint(N->Fe, N->Fm, N->Fprtl, "\tend\n");
		flexprint(N->Fe, N->Fm, N->Fprtl, "endmodule\n");

		// free(tmpPosition);
		// free(fractionValues);

		// for (index = 0; index < targetInvariant->dimensionalMatrixColumnCount; index++) 
		// {
		// 	free(argumentsList[index]);
		// }
		// free(argumentsList);
	}

	flexprint(N->Fe, N->Fm, N->Fprtl, "\n/*\n *\tEnd of the generated .v file\n */\n");
}

void
irPassRTLBackend(State *  N)
{
	FILE *	rtlFile;

	irPassRTLProcessInvariantList(N);

	if (N->outputRTLFilePath) 
	{
		rtlFile = fopen(N->outputRTLFilePath, "w"); 

		if (rtlFile == NULL)
		{
			flexprint(N->Fe, N->Fm, N->Fperr, "\n%s: %s.\n", Eopen, N->outputRTLFilePath);
			consolePrintBuffers(N);
		}

		fprintf(rtlFile, "%s", N->Fprtl->circbuf);
		fclose(rtlFile);
	}
}
