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
#include "noisy-timeStamps.h"
#include "common-timeStamps.h"
#include "common-data-structures.h"
#include "noisy-parser.h"
#include "noisy-lexer.h"
#include "common-irPass-helpers.h"
#include "noisy-types.h"

extern char *	gNoisyAstNodeStrings[];


static bool	isType(State *  N, IrNode *  node);
static char *	scope2id(State *  N, Scope *  scope);
static char *	scope2id2(State *  N, Scope *  scope);
static char *	symbol2id(State *  N, Symbol *  symbol);


int
noisyIrPassDotAstDotFmt(State *  N, char *  buf, int bufferLength, IrNode *  irNode)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyIrPassDotAstDotFmt);

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
	typeString		= &gNoisyAstNodeStrings[irNode->type][strlen("kNoisyIrNodeType_")];

	if (gNoisyAstNodeStrings[irNode->type] == NULL)
	{
		typeString = (char *)EunhandledNodeTypeInAstNodeStringsArray;
	}

	/*
	 *	For identifiers, different graph node properties
	 */
	if (irNode->type == kNoisyIrNodeType_Tidentifier)
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

	src = (char *) calloc(kNoisyMaxPrintBufferLength, sizeof(char));
	if (src == NULL)
	{
		fatal(N, Emalloc);
	}

	if (irNode->type != kNoisyIrNodeType_Xseq)
	{
		snprintf(src, kNoisyMaxPrintBufferLength, "| source:%"PRIu64",%"PRIu64"", irNode->sourceInfo->lineNumber, irNode->sourceInfo->columnNumber);
	}

	if (N->dotDetailLevel & kNoisyDotDetailLevelNoText)
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
			((tokenString == NULL || strlen(tokenString) == 0) ? "" : " tokenString="), tokenString,
			src, (isType(N, irNode) ? "#" : ""), (isType(N, irNode) ? noisyTypeMakeTypeSignature(N, irNode) : ""), (isType(N, irNode) ? "#" : ""), nodeBorderString);
	}

	bufferLength = max(bufferLength - n, 0);

	if (!(N->dotDetailLevel & kNoisyDotDetailLevelNoNilNodes) && (L(irNode) == NULL))
	{
		n += snprintf(&buf[n], bufferLength, "\tP" FLEX_PTRFMTH "_leftnil [%s];\n",
			(FlexAddr)irNode, nilFormatString);
		bufferLength = max(bufferLength - n, 0);
	}
	if (!(N->dotDetailLevel & kNoisyDotDetailLevelNoNilNodes) && (R(irNode) == NULL))
	{
		n += snprintf(&buf[n], bufferLength, "\tP" FLEX_PTRFMTH "_rightnil [%s];\n",
			(FlexAddr)irNode, nilFormatString);
		bufferLength = max(bufferLength - n, 0);
	}

	l = (char *)calloc(kNoisyMaxPrintBufferLength, sizeof(char));
	if (l == NULL)
	{
		fatal(N, Emalloc);
	}

	r = (char *)calloc(kNoisyMaxPrintBufferLength, sizeof(char));
	if (r == NULL)
	{
		fatal(N, Emalloc);
	}

	if (!(N->dotDetailLevel & kNoisyDotDetailLevelNoNilNodes) && (L(irNode) == NULL))
	{
		snprintf(l, kNoisyMaxPrintBufferLength, "P" FLEX_PTRFMTH "_leftnil", (FlexAddr)irNode);
	}
	else if (L(irNode) != NULL)
	{
		snprintf(l, kNoisyMaxPrintBufferLength, "P" FLEX_PTRFMTH "", (FlexAddr)L(irNode));
	}

	if (!(N->dotDetailLevel & kNoisyDotDetailLevelNoNilNodes) && (R(irNode) == NULL))
	{
		snprintf(r, kNoisyMaxPrintBufferLength, "P" FLEX_PTRFMTH "_rightnil", (FlexAddr)irNode);
	}
	else if (R(irNode) != NULL)
	{
		snprintf(r, kNoisyMaxPrintBufferLength, "P" FLEX_PTRFMTH, (FlexAddr)R(irNode));
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
noisyIrPassDotSymbolTableDotFmt(State *  N, char *  buf, int bufferLength, Scope *  scope)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyIrPassDotSymbotTableDotFmt);

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
noisyIrPassDotAstPrintWalk(State *  N, IrNode *  irNode, char *  buf, int bufferLength)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyIrPassAstDotPrintWalk);

	int	n0 = 0, n1 = 0, n2 = 0;

	if (irNode == NULL)
	{
		return 0;
	}

	if (L(irNode) == irNode || R(irNode) == irNode)
	{
		fatal(N, "Immediate cycle in Ir, seen noisyIrPassAstDotPrintWalk()!!\n");

		/*
		 *	Not reached
		 */
		return 0;
	}

	/*
	 *	For DOT, we walk tree in postorder, though it doesn't matter
	 *	either way.
	 */
	n0 = noisyIrPassDotAstPrintWalk(N, L(irNode), &buf[0], bufferLength);
	n1 = noisyIrPassDotAstPrintWalk(N, R(irNode), &buf[n0], bufferLength-n0);

	/*
	 *	Only process nodes once.
	 */
	if (irNode->nodeColor & kNoisyIrNodeColorDotBackendColoring)
	{
		n2 = noisyIrPassDotAstDotFmt(N, &buf[n0+n1], bufferLength-(n0+n1), irNode);
		irNode->nodeColor &= ~kNoisyIrNodeColorDotBackendColoring;
	}

	return (n0+n1+n2);
}


int
noisyIrPassDotSymbolTablePrintWalk(State *  N, Scope *  scope, char *  buf, int bufferLength)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyIrPassSymbolTableDotPrintWalk);

	int	n0 = 0, n1 = 0, n2 = 0;

	if (scope == NULL)
	{
		return 0;
	}

	Scope *	tmp = scope->firstChild;
	while (tmp != NULL)
	{
		n0 += noisyIrPassDotSymbolTablePrintWalk(N, tmp, &buf[n0], (bufferLength-n0));
		tmp = tmp->next;
	}

	/*
	 *	Only process nodes once.
	 */
	if (scope->nodeColor & kNoisyIrNodeColorDotBackendColoring)
	{
		n1 = noisyIrPassDotSymbolTableDotFmt(N, &buf[n0], (bufferLength-n0), scope);
		scope->nodeColor &= ~kNoisyIrNodeColorDotBackendColoring;
	}

	n2 += noisyIrPassDotSymbolTablePrintWalk(N, scope->next, &buf[n0+n1], (bufferLength-(n0+n1)));

	return (n0+n1+n2);
}


char *
noisyIrPassDotBackend(State *  N, Scope *  noisyIrTopScope, IrNode * noisyIrRoot)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyIrPassDotBackend);

	int			bufferLength, irAndSymbolTableSize = 0;
	char *			buf = NULL;
#if defined(NoisyOsMacOSX) || defined(NoisyOsLinux)
	int			n = 0;
	struct timeval		t;

	/*
	 *	Length is required to be 26 chars by ctime_r.
	 */
	char			dateString[26];
#endif



	/*
	 *	Heuristic. Could be better. See #322.
	 */
	irAndSymbolTableSize += irPassHelperIrSize(N, noisyIrRoot);
	irAndSymbolTableSize += irPassHelperSymbolTableSize(N, noisyIrTopScope);
	bufferLength = irAndSymbolTableSize*kNoisyChunkBufferLength;

	/*
	 *	This buffer is deallocated by our caller
	 */
	buf = calloc(bufferLength, sizeof(char));
	if (buf == NULL)
	{
		fatal(N, Emalloc);
	}

#if defined(NoisyOsMacOSX) || defined(NoisyOsLinux)
	gettimeofday(&t, NULL);
	ctime_r(&t.tv_sec, dateString);

	/*
	 *	Ctime string always ends in '\n\0'; elide the '\n'
	 */
	dateString[24] = '.';

	n += snprintf(&buf[n], bufferLength, "digraph Noisy\n{\n");

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
	 *	from here, and don't need to have the kNoisyVersion symbol here either.
	 *	See #323.
	 */
	n += snprintf(&buf[n], bufferLength, "\tlabel = \"\\nAuto-generated by Noisy compiler, version %s, on %s\";\n",
			kNoisyVersion, dateString);
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
	irPassHelperColorIr(N, noisyIrRoot, kNoisyIrNodeColorDotBackendColoring, true/* set */, true/* recurse flag */);
	irPassHelperColorSymbolTable(N, noisyIrTopScope, kNoisyIrNodeColorDotBackendColoring, true/* set */, true/* recurse flag */);

	n += noisyIrPassDotAstPrintWalk(N, noisyIrRoot, &buf[n], bufferLength);
	bufferLength = max(bufferLength - n, 0);

	n += noisyIrPassDotSymbolTablePrintWalk(N, noisyIrTopScope, &buf[n], bufferLength);
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
	irPassHelperColorIr(N, noisyIrRoot, ~kNoisyIrNodeColorDotBackendColoring, false/* clear */, true/* recurse flag */);
	irPassHelperColorSymbolTable(N, noisyIrTopScope, ~kNoisyIrNodeColorDotBackendColoring, false/* clear */, true/* recurse flag */);
#endif

	return buf;
}



/*
 *	Local functions.
 */



static bool
isType(State *  N, IrNode *  node)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyIrPassDotIsType);

	switch (node->type)
	{
		case kNoisyIrNodeType_PaccuracyTolerance:
		case kNoisyIrNodeType_PadtTypeDecl:
		case kNoisyIrNodeType_PanonAggregateType:
		case kNoisyIrNodeType_ParrayType:
		case kNoisyIrNodeType_PcomplexType:
		case kNoisyIrNodeType_PconstantDecl:
		case kNoisyIrNodeType_PfixedType:
		case kNoisyIrNodeType_PfunctionDecl:
		case kNoisyIrNodeType_PlatencyTolerance:
		case kNoisyIrNodeType_PlistType:
		case kNoisyIrNodeType_PlossTolerance:
		case kNoisyIrNodeType_PmoduleDecl:
		case kNoisyIrNodeType_PrationalType:
		case kNoisyIrNodeType_PtupleType:
		case kNoisyIrNodeType_PtypeDecl:
		case kNoisyIrNodeType_PtypeExpr:
		case kNoisyIrNodeType_PtypeName:
		case kNoisyIrNodeType_PtypeParameterList:
		case kNoisyIrNodeType_Tadt:
		case kNoisyIrNodeType_Talpha:
		case kNoisyIrNodeType_Tbool:
		case kNoisyIrNodeType_Tconst:
		case kNoisyIrNodeType_Tepsilon:
		case kNoisyIrNodeType_Tfixed:
		case kNoisyIrNodeType_Tfloat128:
		case kNoisyIrNodeType_Tfloat16:
		case kNoisyIrNodeType_Tfloat32:
		case kNoisyIrNodeType_Tfloat4:
		case kNoisyIrNodeType_Tfloat64:
		case kNoisyIrNodeType_Tfloat8:
		case kNoisyIrNodeType_Tfunction:
		case kNoisyIrNodeType_Tidentifier:
		case kNoisyIrNodeType_Tint128:
		case kNoisyIrNodeType_Tint16:
		case kNoisyIrNodeType_Tint32:
		case kNoisyIrNodeType_Tint4:
		case kNoisyIrNodeType_Tint64:
		case kNoisyIrNodeType_Tint8:
		case kNoisyIrNodeType_Tlist:
		case kNoisyIrNodeType_Tnat128:
		case kNoisyIrNodeType_Tnat16:
		case kNoisyIrNodeType_Tnat32:
		case kNoisyIrNodeType_Tnat4:
		case kNoisyIrNodeType_Tnat64:
		case kNoisyIrNodeType_Tnat8:
		case kNoisyIrNodeType_TrealConst:
		case kNoisyIrNodeType_Tset:
		case kNoisyIrNodeType_Tstring:
		case kNoisyIrNodeType_Ttau:
 		case kNoisyIrNodeType_Ttype:
		case kNoisyIrNodeType_PbasicType:
		case kNoisyIrNodeType_PintegerType:
		case kNoisyIrNodeType_PwriteTypeSignature:
		case kNoisyIrNodeType_Psignature:
		case kNoisyIrNodeType_PreadTypeSignature:
		case kNoisyIrNodeType_PmoduleDeclBody:
		case kNoisyIrNodeType_PmoduleTypeNameDecl:	
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


static char *
scope2id(State *  N, Scope *  scope)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyIrPassDotScope2Id);

	if (scope == NULL)
	{
		fatal(N, Esanity);
	}
	if (scope->begin == NULL || scope->end == NULL)
	{
		return "???";
	}

	char *	buf, tmp[kNoisyMaxBufferLength];

	int length = snprintf(tmp, kNoisyMaxBufferLength, "%"PRIu64"_%"PRIu64"_%"PRIu64"_%"PRIu64"",
			scope->begin->lineNumber, scope->begin->columnNumber,
			scope->end->lineNumber, scope->end->columnNumber);

	buf = (char *)malloc(length);

	sprintf(buf, "%"PRIu64"_%"PRIu64"_%"PRIu64"_%"PRIu64"",
			scope->begin->lineNumber, scope->begin->columnNumber,
			scope->end->lineNumber, scope->end->columnNumber);

	return buf;
}


static char *
scope2id2(State *  N, Scope *  scope)
{
	TimeStampTraceMacro(kNoisyTimeStampKeyIrPassDotScope2Id2);

	if (scope == NULL)
	{
		fatal(N, Esanity);
	}
	if (scope->begin == NULL || scope->end == NULL)
	{
		return "???";
	}

	char *	buf, tmp[kNoisyMaxBufferLength];

	int length = snprintf(tmp, kNoisyMaxBufferLength, "%s:%"PRIu64",%"PRIu64" \\nto\\n %s:%"PRIu64",%"PRIu64"",
		scope->begin->fileName, scope->begin->lineNumber, 
		scope->begin->columnNumber,  scope->begin->fileName,
		scope->end->lineNumber, scope->end->columnNumber);

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
	TimeStampTraceMacro(kNoisyTimeStampKeyIrPassDotSymbol2Id);

	if (symbol == NULL)
	{
		fatal(N, Esanity);
	}
	if (symbol->sourceInfo == NULL)
	{
		return "???";
	}

	char *	buf, tmp[kNoisyMaxBufferLength];

	int length = snprintf(tmp, kNoisyMaxBufferLength, "%"PRIu64"_%"PRIu64"",
		symbol->sourceInfo->lineNumber, symbol->sourceInfo->columnNumber);

	buf = (char *)malloc(length);

	sprintf(buf, "%"PRIu64"_%"PRIu64"",
		symbol->sourceInfo->lineNumber, symbol->sourceInfo->columnNumber);

	return buf;
	
}
