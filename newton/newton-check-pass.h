void
newtonCheckCompareOp(
    State* N,
    IrNode * leftExpression, 
    IrNode * rightExpression,
    IrNodeType compareOpType,
    ConstraintReport * report
);

void
newtonCheckBinOp(
    State* N,
    IrNode * leftTerm, 
    IrNode * rightTerm,
    IrNodeType binOpType,
    ConstraintReport * report
); 

void
checkSingleConstraint(
    State * N,
    IrNode * constraintTreeRoot,
    IrNode* parameterTreeRoot,
    NewtonAPIReport* report
);

double
checkQuantityExpression(
    State * N,
    IrNode * constraintTreeRoot,
    IrNode * parameterTreeRoot,
    ConstraintReport * report
);

double
checkQuantityTerm(
    State * N,
    IrNode * constraintTreeRoot,
    IrNode * parameterTreeRoot,
    ConstraintReport * report
);

void
checkQuantityFactor(
    State * N,
    IrNode * constraintTreeRoot,
    IrNode * parameterTreeRoot,
    ConstraintReport * report
);

