/*
    Authored 2018. Zhengyang Gu.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    *   Redistributions of source code must retain the above
        copyright notice, this list of conditions and the following
        disclaimer.

    *   Redistributions in binary form must reproduce the above
        copyright notice, this list of conditions and the following
        disclaimer in the documentation and/or other materials
        provided with the distribution.

    *   Neither the name of the author nor the names of its
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
#include "common-irPass-helpers.h"
#include "common-astTransform.h"
#include "common-irHelpers.h"

/*
 * In the comments below, trees are written as (ROOT LEFT_CHILD RiGHT_CHILD)
 * Node names in UPPER CASE are usually auxiliary nodes the nature of which
 * can be ignored for this purpose. Node names in 'single quotes' are the "exact"
 * content of the node ("exact" as in kNewtonIrNodeType_Tminus will be written as
 * '-'). Node names in camelCase are meant to represent the type of the node.
 */


IrNode *  binaryOpTreeTransform(State *  N, IrNode *  inputAST)
{
    /*
     * In a Newton AST, a binary operator tree represents the application of
     * n operators of the same precedence on n+1 operands, e.g. a1 - a2 + a3,
     * a1 / a2 * a3, ..., where a1, a2 and a3 could be expressions with higher
     * precedence.
     * 
     * In the binary operator tree, it is represented in a structure similar to
     * a linked list, e.g., a1 - a2 is (AUX a1 (X_SEQ / (X_SEQ a2 NILL)).
     * 
     * In order to handle the precedence correctly, this function recurses on
     * the left operand, as +,-,*,/ are all left recursive. In the example of
     * a1 + a2 - a3 - a4, the function essentially considers it to be 
     * (((a1 + a2) - a3) - a4). It handles the innerset (a1 + a2) in the first call,
     * and processes the expression in the parenthesis one level higher in each
     * iteration, until it reaches the outerest level.
     */
    IrNodeType opType = RLL(inputAST)->type;
    if (opType == kNewtonIrNodeType_PhighPrecedenceOperator)
    {
        opType = RLLL(inputAST)->type;
    }
    IrNode *  leftOperand = commonTreeTransform(N, L(inputAST));
    IrNode *  rightOperand = commonTreeTransform(N, RRL(inputAST));

    IrNode *  newLeftOperand = genIrNode(N, opType, leftOperand, rightOperand, inputAST->sourceInfo);
    irPassHelperColorIr(N, newLeftOperand, kCommonIrNodeColorTreeTransformedColoring, true, false);

    IrNode *  newRoot = R(R(inputAST));
    /*
     * This tests if there are any trailing operations
     */
    if (R(newRoot) == NULL)
    {
        return newLeftOperand;
    }
    else
    {
        return binaryOpTreeTransform(N, genIrNode(N, newRoot->type, newLeftOperand, R(newRoot), newRoot->sourceInfo));
    }
}

IrNode *  expandMinus(State *  N, IrNode *  rootNode)
{
    /*
     * With the current parser, a quantity term of -1 will be represented as
     * (TERM '-' (X_SEQ '1' NILL)), where TERM is the quantity_term auxiliary
     * node, if more items follows the -1, they will be attached to where the 
     * NILL node currently is.
     * 
     * As a result, it "breaks" the normal structure of binary operation tree,
     * where L(R(root)) is the operator (it would be '1' in the -1 case). It is
     * therefore necessary to shrink it to a single node, in the format of 
     * ('-' NILL '1')so that we can use binaryOpTreeTransform.
     * 
     * NB: the number is stored in the right child since it is more-or-less 0-1
     */

    IrNode *  operand;

    if (R(rootNode)->type == kNoisyIrNodeType_Xseq)
    {
        operand = L(R(rootNode));
    }
    else
    {
        operand = R(rootNode);
    }

    IrNode *  expandedTree = genIrNode(N, kNewtonIrNodeType_Tminus, NULL, \
                                       commonTreeTransform(N, operand),\
                                       L(rootNode)->sourceInfo);

    irPassHelperColorIr(N, expandedTree, kCommonIrNodeColorTreeTransformedColoring, true, false);
    return genIrNode(N, rootNode->type, expandedTree, R(R(rootNode)), rootNode->sourceInfo);
}


IrNode *  commonTreeTransform(State *  N, IrNode *  inputAST)
{
    if (inputAST->nodeColor & kCommonIrNodeColorTreeTransformedColoring)
    {
        return inputAST;
    }
    switch(inputAST->type)
    {
        case kNewtonIrNodeType_Pconstraint:
        {
            return binaryOpTreeTransform(N, inputAST);
        }
        case kNewtonIrNodeType_PquantityExpression:
        {
            /*
             * When it is a single quantity term
             */
            if (R(inputAST) == NULL)
            {
                return commonTreeTransform(N, L(inputAST));
            }
            /*
             * When it is a sum of quantity expressions
             */
            else if (R(inputAST)->type == kNoisyIrNodeType_Xseq)
            {
                return binaryOpTreeTransform(N, inputAST);
            }
            else
            {
                fatal(N, "Unrecognized Expression AST Structure!");
            }
            break;
        }
        case kNewtonIrNodeType_PquantityTerm:
        {
            if (R(inputAST) == NULL)
            {
                return commonTreeTransform(N, L(inputAST));
            }
            else if (L(inputAST)->type == kNewtonIrNodeType_Tminus &&
                     !(L(inputAST)->nodeColor & kCommonIrNodeColorTreeTransformedColoring))
            {
                return commonTreeTransform(N, expandMinus(N, inputAST));
            }
            else if (R(inputAST)->type == kNoisyIrNodeType_Xseq)
            {
                return binaryOpTreeTransform(N, inputAST);
            }
            else
            {
                fatal(N, "Unrecognized Term AST Structure!");
            }
            break;
        }
        case kNewtonIrNodeType_PquantityFactor:
        {
            /*
             * When it is a single quantity term
             */
            if (R(inputAST) == NULL)
            {
                return commonTreeTransform(N, L(inputAST));
            }
            /*
             * When it is a sum of quantity expressions
             */
            else if (RL(inputAST)->type == kNewtonIrNodeType_PexponentiationOperator)
            {
                return binaryOpTreeTransform(N, inputAST);
            }
            else
            {
                fatal(N, "Unrecognized Expression AST Structure!");
            }
            break;
        }

        case kNewtonIrNodeType_Pquantity:
        {
            /*
             * When it is a single quantity
             */
            if (R(inputAST) == NULL)
            {
                return commonTreeTransform(N, L(inputAST));
            }
        }

        case kNewtonIrNodeType_PnumericConst:
        {
            if (R(inputAST) == NULL)
            {
                return commonTreeTransform(N, L(inputAST));
            }
        }

        case kNewtonIrNodeType_Tidentifier:
        case kNewtonIrNodeType_TrealConst:
        case kNewtonIrNodeType_TintegerConst:
        {
            return inputAST;
        }
        default:
        {
            fatal(N, "Unknown AST Root Node Type!");
        }
    }
}
