NoisyState * newtonApiInit(char *  newtonFileName);
Physics* newtonApiGetPhysicsTypeByName(NoisyState* N, char* nameOfType);
NewtonAPIReport* newtonApiSatisfiesConstraints(NoisyState* N, NoisyIrNode* parameterTreeRoot);
Invariant * newtonApiGetInvariantByParameters(NoisyState* N, NoisyIrNode* parameterTreeRoot);