#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "flextypes.h"
#include "flexerror.h"
#include "flex.h"

#include "common-timeStamps.h"
#include "data-structures.h"
#include "newton-data-structures.h"
#include "common-irHelpers.h"
#include "common-lexers-helpers.h"
#include "newton-parser.h"
#include "newton-parser-expression.h"
#include "newton-lexer.h"
#include "newton-symbolTable.h"
#include "newton-api.h"

#include "minunit.h"
#include "test-newton-api.h"

extern int tests_run;

static int numberOfConstraintsPassed(NewtonAPIReport* newtonReport);

static int
numberOfConstraintsPassed(NewtonAPIReport* newtonReport)
{
  int count = 0;
  ConstraintReport* current = newtonReport->firstConstraintReport;

  while (current != NULL)
    {
      printf("satisfiesValueConstraint %d\n", current->satisfiesValueConstraint);
      printf("satisfiesDimensionConstraint %d\n\n", current->satisfiesDimensionConstraint);
      printf("valueErrorMessage %s\n", current->valueErrorMessage);
      printf("dimensionErrorMessage %s\n\n", current->dimensionErrorMessage);
      if (current->satisfiesValueConstraint && current->satisfiesDimensionConstraint)
        count++;
      current = current->next;
    }

  return count;
}

char * test_newtonApiInit_notNull()
{
	mu_assert(
        "test_newtonApiInit_notNull: newtonApiInit returns NULL!",
        newtonApiInit("../Examples/invariants.nt") != NULL
    );
    return 0;
}

char * test_newtonApiInit_notNullInvariant()
{
    State* N = newtonApiInit("../Examples/invariants.nt");
	mu_assert(
        "test_newtonApiInit_notNullInvariant: invariantList is NULL!",
         N->invariantList != NULL
    );
    return 0;
}

char * test_newtonApiGetPhysicsTypeByName_Valid()
{
    State* N = newtonApiInit("../Examples/invariants.nt");
    mu_assert(
        "newtonApiGetPhysicsTypeByName: distance not found",
        !strcmp(
            newtonApiGetPhysicsTypeByName(N, "distance")->identifier,
            "distance"
        )
    );

    mu_assert(
        "newtonApiGetPhysicsTypeByName: acceleration not found",
        !strcmp(
            newtonApiGetPhysicsTypeByName(N, "acceleration")->identifier,
            "acceleration"
        )
    );

    return 0;
}

char * test_newtonApiGetInvariantByParameters_Valid()
{
    State* N = newtonApiInit("../Examples/invariants.nt");

    mu_assert(
        "test_newtonApiGetInvariantByParameters: the invariant is named SimplePendulum",
        !strcmp(
            newtonApiGetInvariantByParameters(
                N,
                N->invariantList->parameterList
            )->identifier,
            "SimplePendulum"
        )
    );
    return 0;
}

char * test_newtonCheckSingleInvariant()
{
	State * newton = newtonApiInit("../Examples/invariants.nt");
	IrNode* parameterTree = makeTestParameterTuple(newton);
	NewtonAPIReport* newtonReport = newtonApiSatisfiesConstraints(
		newton,
		parameterTree
		);
	int numberPassed = numberOfConstraintsPassed(newtonReport);

	mu_assert(
		"test_newtonCheckSingleInvariant: number passed should be 5",
		numberPassed == 5
		);
	return 0;
}

char * test_newtonApiDimensionCheckTree()
{
	State * newton = newtonApiInit("../Examples/invariants.nt");

	ConstraintReport* report = newtonApiDimensionCheckTree(newton, makeSampleCorrectTestStatement());
	assert(report->satisfiesDimensionConstraint);
	mu_assert(
		"correct test statements should pass dimension constraint",
		report->satisfiesDimensionConstraint
		);

	report = newtonApiDimensionCheckTree(newton, makeSampleIncorrectTestStatement());
	assert(!report->satisfiesDimensionConstraint);
	mu_assert(
		"incorrect test statements should not pass dimension constraint",
		!report->satisfiesDimensionConstraint
		);

	return 0;
}

/*
 * This example shows how a host language compiler (e.g. Noisy compiler) would check 
 * if two IR nodes are dimensionally equivalent during the parsing step.
 * Let's say the Noisy compiler encounters two variables of type distance and time.
 * The Noisy compiler can look up Physics * struct's corresponding to distance and time
 * and then compare the ID's to see if they should be equal.
 */
char * test_newtonApiPhysicsTypeUsageExample()
{
    State * noisy = init(kNoisyModeDefault);
    State * newton = newtonApiInit("../Examples/invariants.nt");

    IrNode * distanceNode = makeIrNodeSetValue(
        noisy,
        kNoisyIrNodeType_Tidentifier,
        "distance",
        0.0
    );

    IrNode * timeNode = makeIrNodeSetValue(
        noisy,
        kNoisyIrNodeType_Tidentifier,
        "time",
        0.0
    );

    distanceNode->physics = newtonApiGetPhysicsTypeByName(newton, distanceNode->token->identifier);
    timeNode->physics = newtonApiGetPhysicsTypeByName(newton, timeNode->token->identifier);

    mu_assert(
        "test_newtonApiTypeExpressionExample: time and distance id's should be different and cannot be used in add or subtract",
        distanceNode->physics->id != timeNode->physics->id
    );

    return 0;
}


char * test_newtonApiNumberParametersZeroToN()
{
	State * newton = newtonApiInit("../Examples/invariants.nt");
	IrNode* parameterTree = makeTestParameterTuple(newton);
	mu_assert(
		"test_newtonApiNumberParametersZeroToN: the first left child should have number of 0",
		parameterTree->irLeftChild->parameterNumber == 0
		);
	mu_assert(
		"test_newtonApiNumberParametersZeroToN: the first right child should have number of 1",
		parameterTree->irRightChild->irLeftChild->parameterNumber == 1
		);
	return 0;
}

/*
 * Constructs an example tree for the statement
 * foo / bar + fizz / bazz = foo / bar + fizz * bazz, where
 * distance foo, bar = 8;
 * time fizz, bazz = 2;
 *
 */
IrNode *
makeSampleIncorrectTestStatement()
{
    State * newton = newtonApiInit("../Examples/invariants.nt");
	IrNode * root = genIrNode(newton,
							  kNewtonIrNodeType_PquantityStatement,
							  NULL,
							  NULL,
							  NULL);

	newtonApiAddLeaf(newton,
					 root,
					 makeSampleCorrectTestExpression()
		);

	newtonApiAddLeafWithChainingSeqNoLexer(newton,
					 root,
					 genIrNode(newton,
							   kNewtonIrNodeType_Tequals,
							   NULL,
							   NULL,
							   NULL)
		);

	newtonApiAddLeaf(newton,
					 root,
					 makeSampleIncorrectTestExpression()
		);
	return root;
}

/*
 * Constructs an example tree for the expression
 * foo / bar + fizz * bazz, where
 * distance foo, bar = 8;
 * time fizz, bazz = 2;
 */
IrNode *
makeSampleIncorrectTestExpression()
{
    State * newton = newtonApiInit("../Examples/invariants.nt");
	IrNode * root = genIrNode(newton,
							  kNewtonIrNodeType_PquantityExpression,
							  NULL,
							  NULL,
							  NULL);

	IrNode * leftTerm = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_PquantityTerm,
		NULL,
		0
		);
	IrNode * foo = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tidentifier,
		"distance",
		8
		);
	IrNode * div = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tdiv,
		NULL,
		0
		);
	IrNode * bar = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tidentifier,
		"time",
		2
		);
    foo->physics = newtonApiGetPhysicsTypeByName(newton, foo->token->identifier);
	newtonApiAddLeaf(newton, leftTerm, foo);
	newtonApiAddLeafWithChainingSeqNoLexer(newton, leftTerm, div);
    bar->physics = newtonApiGetPhysicsTypeByName(newton, bar->token->identifier);
	newtonApiAddLeaf(newton, leftTerm, bar);

	IrNode * add = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tplus,
		NULL,
		0
		);
	IrNode * rightTerm = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_PquantityTerm,
		NULL,
		0
		);
	IrNode * fizz = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tidentifier,
		"distance",
		8
		);
	IrNode * mul = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tmul,
		NULL,
		0
		);
	IrNode * bazz = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tidentifier,
		"time",
		2
		);
    fizz->physics = newtonApiGetPhysicsTypeByName(newton, fizz->token->identifier);
	newtonApiAddLeaf(newton, rightTerm, fizz);
	newtonApiAddLeafWithChainingSeqNoLexer(newton, rightTerm, mul);
    bazz->physics = newtonApiGetPhysicsTypeByName(newton, bazz->token->identifier);
	newtonApiAddLeaf(newton, rightTerm, bazz);

	newtonApiAddLeaf(newton, root, leftTerm);
	newtonApiAddLeafWithChainingSeqNoLexer(newton, root, add);
	newtonApiAddLeaf(newton, root, rightTerm);

	return root;
}

/*
 * Constructs an example tree for the statement
 * foo / bar + fizz / bazz = foo / bar + fizz / bazz, where
 * distance foo, bar = 8;
 * time fizz, bazz = 2;
 *
 */
IrNode *
makeSampleCorrectTestStatement()
{
    State * newton = newtonApiInit("../Examples/invariants.nt");
	IrNode * root = genIrNode(newton,
							  kNewtonIrNodeType_PquantityStatement,
							  NULL,
							  NULL,
							  NULL);

	newtonApiAddLeaf(newton,
					 root,
					 makeSampleCorrectTestExpression()
		);

	newtonApiAddLeafWithChainingSeqNoLexer(newton,
					 root,
					 genIrNode(newton,
							   kNewtonIrNodeType_Tequals,
							   NULL,
							   NULL,
							   NULL)
		);

	newtonApiAddLeaf(newton,
					 root,
					 makeSampleCorrectTestExpression()
		);
	return root;
}

/*
 * Constructs an example tree for the expression
 * foo / bar + fizz / bazz, where
 * distance foo, bar = 8;
 * time fizz, bazz = 2;
 */
IrNode *
makeSampleCorrectTestExpression()
{
    State * newton = newtonApiInit("../Examples/invariants.nt");
	IrNode * root = genIrNode(newton,
							  kNewtonIrNodeType_PquantityExpression,
							  NULL,
							  NULL,
							  NULL);

	IrNode * leftTerm = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_PquantityTerm,
		NULL,
		0
		);
	IrNode * foo = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tidentifier,
		"distance",
		8
		);
	IrNode * div = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tdiv,
		NULL,
		0
		);
	IrNode * bar = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tidentifier,
		"time",
		2
		);
    foo->physics = newtonApiGetPhysicsTypeByName(newton, foo->token->identifier);
	newtonApiAddLeaf(newton, leftTerm, foo);
	newtonApiAddLeafWithChainingSeqNoLexer(newton, leftTerm, div);
    bar->physics = newtonApiGetPhysicsTypeByName(newton, bar->token->identifier);
	newtonApiAddLeaf(newton, leftTerm, bar);

	IrNode * add = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tplus,
		NULL,
		0
		);
	IrNode * rightTerm = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_PquantityTerm,
		NULL,
		0
		);
	IrNode * fizz = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tidentifier,
		"distance",
		8
		);
	IrNode * div2 = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tdiv,
		NULL,
		0
		);
	IrNode * bazz = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Tidentifier,
		"time",
		2
		);
    fizz->physics = newtonApiGetPhysicsTypeByName(newton, fizz->token->identifier);
	newtonApiAddLeaf(newton, rightTerm, fizz);
	newtonApiAddLeafWithChainingSeqNoLexer(newton, rightTerm, div2);
    bazz->physics = newtonApiGetPhysicsTypeByName(newton, bazz->token->identifier);
	newtonApiAddLeaf(newton, rightTerm, bazz);

	newtonApiAddLeaf(newton, root, leftTerm);
	newtonApiAddLeafWithChainingSeqNoLexer(newton, root, add);
	newtonApiAddLeaf(newton, root, rightTerm);

	return root;
}


IrNode *
makeTestParameterTuple(State * newton)
{
	IrNode *	root = genIrNode(newton,	kNewtonIrNodeType_PparameterTuple,
								 NULL /* left child */,
								 NULL /* right child */,
								 NULL /* source info */);
	IrNode * distanceParameter = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Pparameter,
		"distance",
		5
		);
	distanceParameter->physics = newtonApiGetPhysicsTypeByName(newton, distanceParameter->token->identifier);
	newtonApiAddLeaf(newton, root, distanceParameter);

	IrNode * timeParameter = makeIrNodeSetValue(
		newton,
		kNewtonIrNodeType_Pparameter,
		"time",
		6.6
		);
	timeParameter->physics = newtonApiGetPhysicsTypeByName(newton, timeParameter->token->identifier);
	newtonApiAddLeafWithChainingSeqNoLexer(newton, root, timeParameter);

	newtonApiNumberParametersZeroToN(newton, root);
	return root;
}

IrNode *
makeIrNodeSetValue(
    State * N,
    IrNodeType nodeType,
    char * identifier,
    double realConst
) {
	IrNode * node = genIrNode(
        N,
        nodeType,
	    NULL /* left child */,
	    NULL /* right child */,
	    NULL /* source info */
    );

	node->token = lexAllocateToken(
		N,
		nodeType /* type */,
		identifier /* identifier */,
		0	/* integerConst	*/,
		realConst	/* realConst	*/,
		NULL  /* stringConst	*/,
		NULL	/* sourceInfo	*/
		);

	node->value = node->token->integerConst + node->token->realConst; /* this works because either one is always zero */

    return node;
}