State * newtonApiInit(char *  newtonFileName);
Physics* newtonApiGetPhysicsTypeByName(State* N, char* nameOfType);
Physics * newtonApiGetPhysicsTypeByNameAndSubindex(State* N, char* nameOfType, int subindex);
NewtonAPIReport* newtonApiSatisfiesConstraints(State* N, IrNode* parameterTreeRoot);
Invariant * newtonApiGetInvariantByParameters(State* N, IrNode* parameterTreeRoot);

ConstraintReport*
newtonApiDimensionCheckTree(State * N, IrNode* tree);


void
iterateConstraints(
    State * N,
    IrNode * constraintsTreeRoot,
    IrNode* parameterTreeRoot,
    NewtonAPIReport* report
);


void		newtonApiAddLeaf(State *  N, IrNode *  parent, IrNode *  newNode);
void		newtonApiAddLeafWithChainingSeqNoLexer(State *  N, IrNode *  parent, IrNode *  newNode);
void    newtonApiNumberParametersZeroToN(State * N, IrNode * parameterTreeRoot);
