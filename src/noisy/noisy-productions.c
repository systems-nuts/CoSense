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
#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>
#include <stdint.h>
#include "flextypes.h"
#include "flexerror.h"
#include "flex.h"
#include "noisy-timeStamps.h"
#include "common-timeStamps.h"
#include "common-data-structures.h"



/*
 *	Strings representing the various productions, for debugging and error reporting
 */
const char *	gProductionDescriptions[kCommonIrNodeTypeMax] =
{
	[kNoisyIrNodeType_Pcharacter]			= "character",
	[kNoisyIrNodeType_PreservedToken]		= "reserved operator",
	[kNoisyIrNodeType_PzeroNine]			= "character in [\"0\" .. \"9\"]",
	[kNoisyIrNodeType_PoneNine]			= "character in [\"1\" .. \"9\"]",
	[kNoisyIrNodeType_Pradix]			= "radix prefix",
	[kNoisyIrNodeType_PcharConst]			= "character constant",
	[kNoisyIrNodeType_PintConst]			= "integer constant",
	[kNoisyIrNodeType_PboolConst]			= "Boolean constant (\"true\" or \"false\")",
	[kNoisyIrNodeType_PdRealConst]			= "real-valued constant in decimal notation",
	[kNoisyIrNodeType_PeRealConst]			= "real-valued constant in engineering notation",
	[kNoisyIrNodeType_PrealConst]			= "real-valued constant",
	[kNoisyIrNodeType_PstringConst]			= "string constant",
	[kNoisyIrNodeType_PidentifierChar]		= "valid identifier character",
	[kNoisyIrNodeType_Pidentifier]			= "identifier",
	[kNoisyIrNodeType_Pprogram]			= "Noisy language program",
	[kNoisyIrNodeType_PprogtypeDeclaration]		= "progtype declaration",
	[kNoisyIrNodeType_PprogtypeBody]		= "progtype body",
	[kNoisyIrNodeType_PprogtypeTypenameDeclaration]	= "progtype typename declaration",
	[kNoisyIrNodeType_PconstantDeclaration]		= "constant declaration",
	[kNoisyIrNodeType_PtypeDeclaration]		= "type declaration",
	[kNoisyIrNodeType_PadtTypeDeclaration]		= "ADT type declaration",
	[kNoisyIrNodeType_PnamegenDeclaration]		= "namegen declaration",
	[kNoisyIrNodeType_PidentifierOrNil]		= "identifier or nil",
	[kNoisyIrNodeType_PidentifierList]		= "identifier or list of identifiers",
	[kNoisyIrNodeType_PtypeExpression]		= "type expression",
	[kNoisyIrNodeType_Ptypename]			= "type name",
	[kNoisyIrNodeType_Ptolerance]			= "tolerance specification",
	[kNoisyIrNodeType_PerrorMagnitudeTolerance] 	= "error magnitude tolerance specification",
	[kNoisyIrNodeType_PlossTolerance]		= "loss/erasure tolerance specification",
	[kNoisyIrNodeType_PlatencyTolerance]		= "latency tolerance specification",
	[kNoisyIrNodeType_PbasicType]			= "basic type, \"bit\", \"nibble\", \"byte\", \"int\", \"real\", \"fixed\", \"string\"",
	[kNoisyIrNodeType_PrealType]			= "real-valued type, \"real\" or \"fixed\" intconst \".\" intconst",
	[kNoisyIrNodeType_PfixedType]			= "fixed-point type, \"fixed\" intconst \".\" intconst",
	[kNoisyIrNodeType_PanonAggregateType]		= "anonymous aggregate data type (array, list, tuple, or set)",
	[kNoisyIrNodeType_ParrayType]			= "array data type",
	[kNoisyIrNodeType_PlistType]			= "list data type",
	[kNoisyIrNodeType_PtupleType]			= "tuple data type",
	[kNoisyIrNodeType_PsetType]			= "set data type",
	[kNoisyIrNodeType_PinitList]			= "initialization list",
	[kNoisyIrNodeType_PindexedInitList]		= "initialization list",
	[kNoisyIrNodeType_PstarInitList]		= "initialization list",
	[kNoisyIrNodeType_Pelement]			= "initialization list element",
	[kNoisyIrNodeType_PnamegenDefinition]		= "name generator definition",
	[kNoisyIrNodeType_PscopedStatementList]		= "scoped statement list",
	[kNoisyIrNodeType_PstatementList]		= "statement list",
	[kNoisyIrNodeType_Pstatement]			= "program statement",
	[kNoisyIrNodeType_PassignOp]			= "assignment operator",
	[kNoisyIrNodeType_PmatchStatement]		= "match or matchseq statement",
	[kNoisyIrNodeType_PiterationStatement]		= "iteration statement",
	[kNoisyIrNodeType_PguardBody]			= "guarded statement list",
	[kNoisyIrNodeType_Pexpression]			= "expression",
	[kNoisyIrNodeType_PlistCastExpression]		= "list cast expression",
	[kNoisyIrNodeType_PsetCastExpression]		= "set cast expression",
	[kNoisyIrNodeType_ParrayCastExpression]		= "array cast expression",
	[kNoisyIrNodeType_PanonAggregateCastExpression]	= "array/list/tuple or set cast expression",
	[kNoisyIrNodeType_PchanEventExpression]		= "channel event expression",
	[kNoisyIrNodeType_Pchan2nameExpression]		= "chan2name expression",
	[kNoisyIrNodeType_Pvar2nameExpression]		= "var2name expression",
	[kNoisyIrNodeType_Pname2chanExpression]		= "name2chan expression",
	[kNoisyIrNodeType_Pterm]			= "term",
	[kNoisyIrNodeType_Pfactor]			= "factor",
	[kNoisyIrNodeType_PtupleValue]			= "tuple value",
	[kNoisyIrNodeType_PfieldSelect]			= "field selection expression",
	[kNoisyIrNodeType_PhighPrecedenceBinaryOp]	= "high precedence operator: \"*\", \"/\", \"%\", \"^\" or \"::\"",
	[kNoisyIrNodeType_PlowPrecedenceBinaryOp]	= "low precedence operator: \"+\", \"-\", \">>\", \"<<\", \"|\", \"==\", \"!=\", \">\", \"<\", \"<=\", \">=\" \"&&\" or \"||\"",
	[kNoisyIrNodeType_PcmpOp]			= "comparator operator",
	[kNoisyIrNodeType_PbooleanOp]			= "Boolean operator",
	[kNoisyIrNodeType_PunaryOp]			= "unary operator",
};