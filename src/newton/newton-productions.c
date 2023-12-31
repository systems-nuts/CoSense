/*
	Authored 2018. Phillip Stanley-Marbell.

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
#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdint.h>
#include "flextypes.h"
#include "flexerror.h"
#include "flex.h"
#include "newton-timeStamps.h"
#include "common-timeStamps.h"
#include "common-data-structures.h"



/*
 *	Strings representing the various productions, for debugging and error reporting
 */
const char *	gProductionDescriptions[kCommonIrNodeTypeMax] =
{
	[kNewtonIrNodeType_PaccuracyStatement]		= "accuracy statement",
	[kNewtonIrNodeType_ParithmeticCommand]		= "arithmetic command",
	[kNewtonIrNodeType_PbaseSignalDefinition]	= "base signal definition",
	[kNewtonIrNodeType_PcaseStatement]		= "case statement",
	[kNewtonIrNodeType_PcaseStatementList]		= "case statement list",
	[kNewtonIrNodeType_PcomparisonOperator]		= "comparison operator",
	[kNewtonIrNodeType_PconstantDefinition]		= "constant definition",
	[kNewtonIrNodeType_Pconstraint]			= "constraint",
	[kNewtonIrNodeType_PcallParameterTuple]		= "call parameter tuple",
	[kNewtonIrNodeType_PconstraintList]		= "constraint list",
	[kNewtonIrNodeType_PdelayCommand]		= "delay command",
	[kNewtonIrNodeType_PderivationStatement]	= "derivation statement",
	[kNewtonIrNodeType_Pdistribution]		= "distribution",
	[kNewtonIrNodeType_PdistributionFactor]		= "distributionFactor",
	[kNewtonIrNodeType_PerasureValueStatement]	= "erasure value statement",
	[kNewtonIrNodeType_PexponentiationOperator]	= "exponentiation operator",
	[kNewtonIrNodeType_Pexpression]			= "expression",
	[kNewtonIrNodeType_Pfactor]			= "factor",
	[kNewtonIrNodeType_PhighPrecedenceBinaryOp]	= "high-precedence binary operator",
	[kNewtonIrNodeType_PhighPrecedenceOperator]	= "high-precedence operator",
	[kNewtonIrNodeType_PinvariantDefinition]	= "invariant definition",
	[kNewtonIrNodeType_PlanguageSetting]		= "language setting",
	[kNewtonIrNodeType_PlowPrecedenceBinaryOp]	= "low-precedence binary operator",
	[kNewtonIrNodeType_PlowPrecedenceOperator]	= "low-precedence operator",
	[kNewtonIrNodeType_PnameStatement]		= "name statement",
	[kNewtonIrNodeType_PnewtonDescription]		= "Newton description",
	[kNewtonIrNodeType_PnumericConst]		= "numeric constant",
	[kNewtonIrNodeType_PnumericConstTuple]		= "numeric constant tuple",
	[kNewtonIrNodeType_PnumericConstTupleList]	= "numeric constant tuple list",
	[kNewtonIrNodeType_PnumericFactor]		= "numeric factor",
	[kNewtonIrNodeType_PnumericTerm]		= "numeric term",
	[kNewtonIrNodeType_PnumericExpression]		= "numeric expression",
	[kNewtonIrNodeType_Pparameter]			= "parameter",
	[kNewtonIrNodeType_PparameterTuple]		= "parameter tuple",
	[kNewtonIrNodeType_PparameterValueList]		= "parameter value list",
	[kNewtonIrNodeType_PpiecewiseConstraint]	= "piecewise constraint",
	[kNewtonIrNodeType_PprecisionStatement]		= "precision statement",
	[kNewtonIrNodeType_Pquantity]			= "quantity",
	[kNewtonIrNodeType_PquantityExpression]		= "quantity expression",
	[kNewtonIrNodeType_PquantityFactor]		= "quantity factor",
	[kNewtonIrNodeType_PquantityTerm]		= "quantity term",
	[kNewtonIrNodeType_PrangeStatement]		= "range statement",
	[kNewtonIrNodeType_PreadRegisterCommand]	= "read register command",
	[kNewtonIrNodeType_Prule]			= "rule",
	[kNewtonIrNodeType_PruleList]			= "rule list",
	[kNewtonIrNodeType_PsensorStatement]		= "signal sensor statement",
	[kNewtonIrNodeType_PsensorDefinition]		= "sensor definition",
	[kNewtonIrNodeType_PsensorInterfaceCommand]	= "sensor interface command",
	[kNewtonIrNodeType_PsensorInterfaceCommandList]	= "sensor interface command list",
	[kNewtonIrNodeType_PsensorInterfaceStatement]	= "sensor interface statement",
	[kNewtonIrNodeType_PsensorInterfaceType]	= "sensor interface type",
	[kNewtonIrNodeType_PsensorProperty]		= "sensor property",
	[kNewtonIrNodeType_PsensorPropertyList]		= "sensor property list",
	[kNewtonIrNodeType_PsignalUncertaintyStatement]	= "signal uncertainty statement",
	[kNewtonIrNodeType_PsubdimensionTuple]		= "sub-dimension tuple",
	[kNewtonIrNodeType_PsymbolStatement]		= "symbol statement",
	[kNewtonIrNodeType_Pterm]			= "term",
	[kNewtonIrNodeType_Ptranscendental]		= "transcendental",
	[kNewtonIrNodeType_PfunctionalOperator]		= "functional operator",
	[kNewtonIrNodeType_PunaryOp]			= "unary operator",
	[kNewtonIrNodeType_PuncertaintyStatement]	= "uncertainty statement",
	[kNewtonIrNodeType_Punit]			= "unit",
	[kNewtonIrNodeType_PunitExpression]		= "unit expression",
	[kNewtonIrNodeType_PunitFactor]			= "unit factor",
	[kNewtonIrNodeType_PunitTerm]			= "unit term",
	[kNewtonIrNodeType_PvectorOp]			= "vector operation",
	[kNewtonIrNodeType_PwriteRegisterCommand]	= "write register command",
};
