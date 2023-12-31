/*
	Authored 2015, Phillip Stanley-Marbell. Modified 2017, Jonathan Lim.


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
#include <assert.h>
#include <stdlib.h>
#include <setjmp.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "flextypes.h"
#include "flexerror.h"
#include "flex.h"
#include "common-errors.h"
#include "version.h"
#include "newton-timeStamps.h"
#include "common-timeStamps.h"
#include "common-data-structures.h"
#include "noisy-parser.h"
#include "noisy-lexer.h"
#include "common-irPass-helpers.h"
#include "common-irHelpers.h"
#include "newton-types.h"

#ifdef CommonOsLinux
#	include <time.h>
#endif



static bool	isType(State *  N, IrNode *  node);
static char *	scope2id(State *  N, Scope *  scope);
static char *	scope2id2(State *  N, Scope *  scope);
static char *	symbol2id(State *  N, Symbol *  symbol);


int
irPassDotAstDotFmt(State *  N, char *  buf, int bufferLength, IrNode *  irNode, char* astNodeStrings[])
{
	TimeStampTraceMacro(kNewtonTimeStampKeyIrPassDotAstDotFmt);

	char *		nilFormatString;
	char *		tokenString = "";
	char *		typeString;
	char *		l;
	char *		nodeBorderString;
	char *		nodePropertiesString;
	char *		r;
	char *		src;
	int		n = 0;


	/*
	 *	If we run out of space in print buffer, we should
	 *	print a "..." rather than just ending like we do now.
	 *	See  #321.
	 */
	if (irNode->tokenString != NULL)
	{
		tokenString = irNode->tokenString;
	}

	/*
	 *	We use the pointer address of the IrNode *  p to give a unique
	 *	string for each node in the graph. NOTE: dot renders _much_ faster
	 *	if we don't supply a fontname (which it often cannot find anyway)...
	 */
	nilFormatString		= "style=filled,color=\"#003333\",fontcolor=white,fontname=\"LucidaSans-Typewriter\",fontsize=8,width=0.3,height=0.16,fixedsize=true,label=\"nil\", shape=record";
	nodePropertiesString	= "";
	nodeBorderString	= "M";
	typeString		= astNodeStrings[irNode->type];

	if (astNodeStrings[irNode->type] == NULL)
	{
		fatal(N, Esanity);
	}

	/*
	 *	For identifiers, different graph node properties
	 */
	if (irNode->type == kNewtonIrNodeType_Tidentifier)
	{
		nodePropertiesString = "style=filled,color=\"#ccff66\",";
		nodeBorderString = "";
	}

	/*
	 *	For X_SEQ, different graph node properties
	 */
	if (irNode->type == kNoisyIrNodeType_Xseq)
	{
		nodePropertiesString = "style=filled,color=\"#999999\",fixedsize=true,";
		nodeBorderString = "M";

		/*
		 *	X_SEQ is not part of gASTnodeSTRINGS, which is generated by our
		 *	ffi2code tools.
		 */
		typeString = "X_SEQ";
	}

	src = (char *) calloc(kCommonMaxPrintBufferLength, sizeof(char));
	if (src == NULL)
	{
		fatal(N, Emalloc);
	}

	if (irNode->type != kNoisyIrNodeType_Xseq)
	{
		snprintf(src, kCommonMaxPrintBufferLength, "| source:%"PRIu64",%"PRIu64"", irNode->sourceInfo->lineNumber, irNode->sourceInfo->columnNumber);
	}

	if (N->dotDetailLevel & kCommonDotDetailLevelNoText)
	{
		n += snprintf(&buf[n], bufferLength,
			"\tP" FLEX_PTRFMTH " [%sfontsize=8,fontname=\"LucidaSans-Typewriter\",height=0.8,"
			"label=\"{ | {<left> | <right> }}\",shape=%srecord];\n",
			(FlexAddr)irNode, nodePropertiesString, nodeBorderString);
	}
	else
	{
		n += snprintf(&buf[n], bufferLength,
			"\tP" FLEX_PTRFMTH " [%sfontsize=8,height=0.8,fontname=\"LucidaSans-Typewriter\","
			"label=\"{P" FLEX_PTRFMTH "\\ntype=%s\\n%s%s\\n%s %s%s%s| {<left> | <right> }}\",shape=%srecord];\n",
			(FlexAddr)irNode, nodePropertiesString, (FlexAddr)irNode, typeString,
			((tokenString == NULL || ((size_t) strlen(tokenString)) == 0) ? "" : " tokenString="), tokenString,
			src, (isType(N, irNode) ? "#" : ""), (isType(N, irNode) ? newtonTypeMakeTypeSignature(N, irNode) : ""), (isType(N, irNode) ? "#" : ""), nodeBorderString);
	}

	bufferLength = max(bufferLength - n, 0);

	if (!(N->dotDetailLevel & kCommonDotDetailLevelNoNilNodes) && (L(irNode) == NULL))
	{
		n += snprintf(&buf[n], bufferLength, "\tP" FLEX_PTRFMTH "_leftnil [%s];\n",
			(FlexAddr)irNode, nilFormatString);
		bufferLength = max(bufferLength - n, 0);
	}
	if (!(N->dotDetailLevel & kCommonDotDetailLevelNoNilNodes) && (R(irNode) == NULL))
	{
		n += snprintf(&buf[n], bufferLength, "\tP" FLEX_PTRFMTH "_rightnil [%s];\n",
			(FlexAddr)irNode, nilFormatString);
		bufferLength = max(bufferLength - n, 0);
	}

	l = (char *)calloc(kCommonMaxPrintBufferLength, sizeof(char));
	if (l == NULL)
	{
		fatal(N, Emalloc);
	}

	r = (char *)calloc(kCommonMaxPrintBufferLength, sizeof(char));
	if (r == NULL)
	{
		fatal(N, Emalloc);
	}

	if (!(N->dotDetailLevel & kCommonDotDetailLevelNoNilNodes) && (L(irNode) == NULL))
	{
		snprintf(l, kCommonMaxPrintBufferLength, "P" FLEX_PTRFMTH "_leftnil", (FlexAddr)irNode);
	}
	else if (L(irNode) != NULL)
	{
		snprintf(l, kCommonMaxPrintBufferLength, "P" FLEX_PTRFMTH "", (FlexAddr)L(irNode));
	}

	if (!(N->dotDetailLevel & kCommonDotDetailLevelNoNilNodes) && (R(irNode) == NULL))
	{
		snprintf(r, kCommonMaxPrintBufferLength, "P" FLEX_PTRFMTH "_rightnil", (FlexAddr)irNode);
	}
	else if (R(irNode) != NULL)
	{
		snprintf(r, kCommonMaxPrintBufferLength, "P" FLEX_PTRFMTH, (FlexAddr)R(irNode));
	}

	if (strlen(l))
	{
		n += snprintf(&buf[n], bufferLength, "\tP" FLEX_PTRFMTH ":left -> %s;\n", (FlexAddr)irNode, l);
		bufferLength = max(bufferLength - n, 0);
	}

	if (strlen(r))
	{
		n += snprintf(&buf[n], bufferLength, "\tP" FLEX_PTRFMTH ":right -> %s;\n", (FlexAddr)irNode, r);
		bufferLength = max(bufferLength - n, 0);
	}

	USED(bufferLength);
	free(l);
	free(r);
	free(src);


	return n;
}


int
irPassDotSymbolTableDotFmt(State *  N, char *  buf, int bufferLength, Scope *  scope)
{
	TimeStampTraceMacro(kNewtonTimeStampKeyIrPassDotSymbotTableDotFmt);

	char *		nilFormatString;
	char *		symbolFormatString;
	int		n = 0;


	/*
	 *	If we run out of space in print buffer, we should
	 *	print a "..." rather than just ending like we do now.
	 *	See  #321.
	 */

	nilFormatString		= "style=filled,color=\"#000000\",fontcolor=white,fontsize=8,width=0.3,height=0.16,fixedsize=true,fontname=\"LucidaSans-Typewriter\",label=\"nil\", shape=record";
	symbolFormatString	= "style=filled,color=\"#dddddd\",fontcolor=black,fontsize=8,height=0.16,fontname=\"LucidaSans-Typewriter\", shape=record";

	n += snprintf(	&buf[n], bufferLength, "\tscope%s [style=filled,color=\"#FFCC00\",fontname=\"LucidaSans-Typewriter\",fontsize=8,height=0.8,label=\"{%s | {<children> | <syms>}}\",shape=record];\n",
			scope2id(N, scope), scope2id2(N, scope));
	bufferLength = max(bufferLength - n, 0);

	if (scope->firstChild == NULL)
	{
		n += snprintf(&buf[n], bufferLength, "\tscope%s_kidsnil [%s];\n", scope2id(N, scope), nilFormatString);
		bufferLength = max(bufferLength - n, 0);
	}
	if (scope->firstSymbol == NULL)
	{
		n += snprintf(&buf[n], bufferLength, "\tscope%s_symsnil [%s];\n", scope2id(N, scope), nilFormatString);
		bufferLength = max(bufferLength - n, 0);
	}

	Scope *	childScope  = scope->firstChild;
	while (childScope != NULL)
	{
		n += snprintf(	&buf[n], bufferLength, "\tscope%s [style=filled,color=\"#FFCC00\",fontname=\"LucidaSans-Typewriter\",fontsize=8,height=0.8,label=\"{%s | {<children> | <syms>}}\",shape=record];\n",
				scope2id(N, childScope), scope2id2(N, childScope));
		bufferLength = max(bufferLength - n, 0);

		childScope = childScope->next;
	}

	Symbol *	childSymbol = scope->firstSymbol;
	while (childSymbol != NULL)
	{
		n += snprintf(&buf[n], bufferLength, "\tsym%s [%s,label=\"{%s}\",shape=rect];\n",
			symbol2id(N, childSymbol), symbolFormatString, childSymbol->identifier);
		bufferLength = max(bufferLength - n, 0);

		childSymbol = childSymbol->next;
	}


	if (scope->firstChild == NULL)
	{
		n += snprintf(&buf[n], bufferLength, "\tscope%s:children -> scope%s_kidsnil;\n", scope2id(N, scope), scope2id(N, scope));
		bufferLength = max(bufferLength - n, 0);
	}
	else
	{
		childScope  = scope->firstChild;
		while (childScope != NULL)
		{
			n += snprintf(&buf[n], bufferLength, "\tscope%s:children -> scope%s;\n", scope2id(N, scope), scope2id(N, childScope));
			bufferLength = max(bufferLength - n, 0);
			
			childScope = childScope->next;
		}
	}

	if (scope->firstSymbol == NULL)
	{
		n += snprintf(&buf[n], bufferLength, "\tscope%s:syms -> scope%s_symsnil;\n", scope2id(N, scope), scope2id(N, scope));
		bufferLength = max(bufferLength - n, 0);
	}
	else
	{
		childSymbol = scope->firstSymbol;
		n += snprintf(&buf[n], bufferLength, "\tscope%s:syms -> sym%s;\n", scope2id(N, scope), symbol2id(N, childSymbol));
		bufferLength = max(bufferLength - n, 0);

		while (childSymbol != NULL && childSymbol->next != NULL)
		{
			n += snprintf(&buf[n], bufferLength, "\tsym%s:syms -> sym%s;\n", symbol2id(N, childSymbol), symbol2id(N, childSymbol->next));
			bufferLength = max(bufferLength - n, 0);
			childSymbol = childSymbol->next;
		}
	}
	USED(bufferLength);

	return n;
}


int
irPassDotAstPrintWalk(State *  N, IrNode *  irNode, char *  buf, int bufferLength, char* astNodeStrings[])
{
	TimeStampTraceMacro(kNewtonTimeStampKeyIrPassAstDotPrintWalk);

	int	n0 = 0, n1 = 0, n2 = 0;

	if (irNode == NULL)
	{
		return 0;
	}

	if (L(irNode) == irNode || R(irNode) == irNode)
	{
		fatal(N, "Immediate cycle in Ir, seen noisyIrPassAstDotPrintWalk()...\n");

		/*
		 *	Not reached
		 */
		return 0;
	}

	/*
	 *	For DOT, we walk tree in postorder, though it doesn't matter
	 *	either way.
	 */
	n0 = irPassDotAstPrintWalk(N, L(irNode), &buf[n0], bufferLength, astNodeStrings);
	n1 = irPassDotAstPrintWalk(N, R(irNode), &buf[n0], bufferLength-n0, astNodeStrings);

	/*
	 *	Only process nodes once.
	 */
	if (irNode->nodeColor & kCommonIrNodeColorDotBackendColoring)
	{
		if(astNodeStrings[irNode->type] == NULL)
		{
			fatal(N, "Unknown node type seen in noisyIrPassAstDotPrintWalk()...\n");
		}

		n2 = irPassDotAstDotFmt(N, &buf[n0+n1], bufferLength-(n0+n1), irNode, astNodeStrings);
		irNode->nodeColor &= ~kCommonIrNodeColorDotBackendColoring;
	}

	return (n0+n1+n2);
}


int
irPassDotSymbolTablePrintWalk(State *  N, Scope *  scope, char *  buf, int bufferLength)
{
	TimeStampTraceMacro(kNewtonTimeStampKeyIrPassSymbolTableDotPrintWalk);

	int	n0 = 0, n1 = 0, n2 = 0;

	if (scope == NULL)
	{
		return 0;
	}

	Scope *	tmp = scope->firstChild;
	while (tmp != NULL)
	{
		n0 += irPassDotSymbolTablePrintWalk(N, tmp, &buf[n0], (bufferLength-n0));
		tmp = tmp->next;
	}

	/*
	 *	Only process nodes once.
	 */
	if (scope->nodeColor & kCommonIrNodeColorDotBackendColoring)
	{
		n1 = irPassDotSymbolTableDotFmt(N, &buf[n0], (bufferLength-n0), scope);
		scope->nodeColor &= ~kCommonIrNodeColorDotBackendColoring;
	}

	n2 += irPassDotSymbolTablePrintWalk(N, scope->next, &buf[n0+n1], (bufferLength-(n0+n1)));

	return (n0+n1+n2);
}


char *
irPassDotBackend(State *  N, Scope *  noisyIrTopScope, IrNode * noisyIrRoot, char* astNodeStrings[])
{
	TimeStampTraceMacro(kNewtonTimeStampKeyIrPassDotBackend);

	int			bufferLength, irAndSymbolTableSize = 0;
	char *			buf = NULL;



	/*
	 *	Heuristic. Could be better. See #322.
	 */
	irAndSymbolTableSize += irPassHelperIrSize(N, noisyIrRoot);
	irAndSymbolTableSize += irPassHelperSymbolTableSize(N, noisyIrTopScope);
	bufferLength = irAndSymbolTableSize*kCommonChunkBufferLength;

	/*
	 *	This buffer is deallocated by our caller
	 */
	buf = calloc(bufferLength, sizeof(char));
	if (buf == NULL)
	{
		fatal(N, Emalloc);
	}

	/*
	 *	Length is required to be 26 chars by ctime_r.
	 */
	char			dateString[26];
	struct timeval		t;
	gettimeofday(&t, NULL);
	ctime_r(&t.tv_sec, dateString);

	/*
	 *	Ctime string always ends in '\n\0'; elide the '\n'
	 */
	dateString[24] = '.';

	int n = 0;
	n += snprintf(&buf[n], bufferLength, "digraph Newton\n{\n");
	bufferLength = max(bufferLength - n, 0);

	/*
	 *	When rendering bitmapped, don't restrict size, and
	 *	leave dpi reasonable (~150).
	 */
	n += snprintf(&buf[n], bufferLength, "\tdpi=150;\n");
	bufferLength = max(bufferLength - n, 0);
	n += snprintf(&buf[n], bufferLength, "\tfontcolor=\"#C0C0C0\";\n");
	bufferLength = max(bufferLength - n, 0);
	n += snprintf(&buf[n], bufferLength, "\tfontsize=18;\n");
	bufferLength = max(bufferLength - n, 0);

	/*
	 *	Take the whole of this following string as one of the arguments,
	 *	called, e.g., "dotplotlabel", so we are not calling gettimeofday()
	 *	from here, and don't need to have the kNewtonVersion symbol here either.
	 *	See #323.
	 */
	n += snprintf(&buf[n], bufferLength, "\tlabel = \"\\nAuto-generated by Newton compiler, version %s, on %s\";\n",
			kNewtonVersion, dateString);
	bufferLength = max(bufferLength - n, 0);
	n += snprintf(&buf[n], bufferLength, "\tsplines = true;\n");
	bufferLength = max(bufferLength - n, 0);

	/*
	 *	Don't restrict size when rendering bitmapped
	 */
	//n += snprintf(&buf[n], bufferLength, "\tsize = \"5,9\";\n");
	//bufferLength = max(bufferLength - n, 0);

	n += snprintf(&buf[n], bufferLength, "\tmargin = 0.1;\n");
	bufferLength = max(bufferLength - n, 0);

	/*
	 *	Temporarily color the graph, so we can know
	 *	which nodes have been visited, in case when
	 *	the graph is not a tree.
	 */
	irPassHelperColorIr(N, noisyIrRoot, kCommonIrNodeColorDotBackendColoring, true/* set */, true/* recurse flag */);
	irPassHelperColorSymbolTable(N, noisyIrTopScope, kCommonIrNodeColorDotBackendColoring, true/* set */, true/* recurse flag */);

	n += irPassDotAstPrintWalk(N, noisyIrRoot, &buf[n], bufferLength, astNodeStrings);
	bufferLength = max(bufferLength - n, 0);

	n += irPassDotSymbolTablePrintWalk(N, noisyIrTopScope, &buf[n], bufferLength);
	bufferLength = max(bufferLength - n, 0);

	n += snprintf(&buf[n], bufferLength, "}\n");
	bufferLength = max(bufferLength - n, 0);
	USED(bufferLength);

	/*
	 *	This is not really necessary, since we now use individual
	 *	bitfields for coloring in different passes, and we clear the
	 *	colors of nodes above anyway. If/when we decide to get rid of
	 *	this, be sure to document the associated gain. See #324.
	 */
	irPassHelperColorIr(N, noisyIrRoot, ~kCommonIrNodeColorDotBackendColoring, false/* clear */, true/* recurse flag */);
	irPassHelperColorSymbolTable(N, noisyIrTopScope, ~kCommonIrNodeColorDotBackendColoring, false/* clear */, true/* recurse flag */);

	return buf;
}



/*
 *	Local functions.
 */



static char *
scope2id(State *  N, Scope *  scope)
{
	TimeStampTraceMacro(kNewtonTimeStampKeyIrPassDotScope2Id);

	if (scope == NULL)
	{
		fatal(N, Esanity);
	}
	if (scope->begin == NULL || scope->end == NULL)
	{
		return "???";
	}

	char *	buf, tmp[kCommonMaxBufferLength];

	int length = snprintf(tmp, kCommonMaxBufferLength, "%"PRIu64"_%"PRIu64"_%"PRIu64"_%"PRIu64"",
			scope->begin->lineNumber, scope->begin->columnNumber,
			scope->end->lineNumber, scope->end->columnNumber) + 1;

	buf = (char *)malloc(length);

	sprintf(buf, "%"PRIu64"_%"PRIu64"_%"PRIu64"_%"PRIu64"",
			scope->begin->lineNumber, scope->begin->columnNumber,
			scope->end->lineNumber, scope->end->columnNumber);

	return buf;
}


static char *
scope2id2(State *  N, Scope *  scope)
{
	TimeStampTraceMacro(kNewtonTimeStampKeyIrPassDotScope2Id2);

	if (scope == NULL)
	{
		fatal(N, Esanity);
	}
	if (scope->begin == NULL || scope->end == NULL)
	{
		return "???";
	}

	char *	buf, tmp[kCommonMaxBufferLength];

	int length = snprintf(tmp, kCommonMaxBufferLength, "%s:%"PRIu64",%"PRIu64" \\nto\\n %s:%"PRIu64",%"PRIu64"",
		scope->begin->fileName, scope->begin->lineNumber, 
		scope->begin->columnNumber,  scope->begin->fileName,
		scope->end->lineNumber, scope->end->columnNumber) + 1;

	buf = (char *)malloc(length);

	sprintf(buf, "%s:%"PRIu64",%"PRIu64" \\nto\\n %s:%"PRIu64",%"PRIu64"",
		scope->begin->fileName, scope->begin->lineNumber, 
		scope->begin->columnNumber,  scope->begin->fileName,
		scope->end->lineNumber, scope->end->columnNumber);

	return buf;
}


static char *
symbol2id(State *  N, Symbol *  symbol)
{
	TimeStampTraceMacro(kNewtonTimeStampKeyIrPassDotSymbol2Id);

	if (symbol == NULL)
	{
		fatal(N, Esanity);
	}
	if (symbol->sourceInfo == NULL)
	{
		return "???";
	}

	char *	buf, tmp[kCommonMaxBufferLength];

	int length = snprintf(tmp, kCommonMaxBufferLength, "%"PRIu64"_%"PRIu64"",
		symbol->sourceInfo->lineNumber, symbol->sourceInfo->columnNumber) + 1;

	buf = (char *)malloc(length);

	sprintf(buf, "%"PRIu64"_%"PRIu64"",
		symbol->sourceInfo->lineNumber, symbol->sourceInfo->columnNumber);

	return buf;

}


static bool
isType(State *  N, IrNode *  node)
{
	TimeStampTraceMacro(kNewtonTimeStampKeyIrPassDotIsType);

	/*
	 *	This is misusing the intent of isType(). See issue #320.
	 */
	switch (node->type)
	{
		case kNewtonIrNodeType_PcomparisonOperator:
		case kNewtonIrNodeType_Pconstraint:
		case kNewtonIrNodeType_PconstraintList:
		case kNewtonIrNodeType_PhighPrecedenceBinaryOp:
		case kNewtonIrNodeType_PhighPrecedenceOperator:
		case kNewtonIrNodeType_PlanguageSetting:
		case kNewtonIrNodeType_PlowPrecedenceBinaryOp:
		case kNewtonIrNodeType_Pparameter:
		case kNewtonIrNodeType_PparameterTuple:
		case kNewtonIrNodeType_Pquantity:
		case kNewtonIrNodeType_PquantityExpression:
		case kNewtonIrNodeType_PquantityFactor:
		case kNewtonIrNodeType_PquantityTerm:
		case kNewtonIrNodeType_PunaryOp:
		case kNewtonIrNodeType_PvectorOp:
		case kNewtonIrNodeType_TEnglish:
		case kNewtonIrNodeType_Tcolon:
		case kNewtonIrNodeType_Tcomma:
		case kNewtonIrNodeType_Tconstant:
		case kNewtonIrNodeType_Tcross:
		case kNewtonIrNodeType_Tderivation:
		case kNewtonIrNodeType_Tderivative:
		case kNewtonIrNodeType_Tdiv:
		case kNewtonIrNodeType_Tdot:
		case kNewtonIrNodeType_Tequals:
		case kNewtonIrNodeType_Tge:
		case kNewtonIrNodeType_Tgt:
		case kNewtonIrNodeType_Tidentifier:
		case kNewtonIrNodeType_TintegerConst:
		case kNewtonIrNodeType_Tintegral:
		case kNewtonIrNodeType_Tinvariant:
		case kNewtonIrNodeType_Tle:
		case kNewtonIrNodeType_TleftBrace:
		case kNewtonIrNodeType_TleftParen:
		case kNewtonIrNodeType_Tlt:
		case kNewtonIrNodeType_Tminus:
		case kNewtonIrNodeType_Tmul:
		case kNewtonIrNodeType_Tname:
		case kNewtonIrNodeType_Tplus:
		case kNewtonIrNodeType_TrealConst:	
		case kNewtonIrNodeType_TrightBrace:
		case kNewtonIrNodeType_TrightParen:
		case kNewtonIrNodeType_Tsemicolon:
		case kNewtonIrNodeType_Tsignal:
		case kNewtonIrNodeType_TstringConst:
		case kNewtonIrNodeType_Tsymbol:
		{
			return true;
		}

		default:
		{
			return false;
		}
	}

	return false;
}
