// See the file "COPYING" in the main distribution directory for copyright.

// Optimization-related methods for Expr classes.

#include "Expr.h"
#include "Stmt.h"
#include "Func.h"
#include "Frame.h"
#include "Scope.h"
#include "Desc.h"
#include "Traverse.h"
#include "Reporter.h"
#include "script_opt/Inline.h"


namespace zeek::detail {


ExprPtr NameExpr::Duplicate()
	{
	return SetSucc(new NameExpr(id, in_const_init));
	}


ExprPtr ConstExpr::Duplicate()
	{
	return SetSucc(new ConstExpr(val));
	}

  
ExprPtr UnaryExpr::Inline(Inliner* inl)
	{
	op = op->Inline(inl);
	return ThisPtr();
	}


ExprPtr BinaryExpr::Inline(Inliner* inl)
	{
	op1 = op1->Inline(inl);
	op2 = op2->Inline(inl);

	return ThisPtr();
	}


ExprPtr CloneExpr::Duplicate()
	{
	// oh the irony
	return SetSucc(new CloneExpr(op->Duplicate()));
	}


ExprPtr IncrExpr::Duplicate()
	{
	return SetSucc(new IncrExpr(tag, op->Duplicate()));
	}


ExprPtr ComplementExpr::Duplicate()
	{
	return SetSucc(new ComplementExpr(op->Duplicate()));
	}


ExprPtr NotExpr::Duplicate()
	{
	return SetSucc(new NotExpr(op->Duplicate()));
	}


ExprPtr PosExpr::Duplicate()
	{
	return SetSucc(new PosExpr(op->Duplicate()));
	}


ExprPtr NegExpr::Duplicate()
	{
	return SetSucc(new NegExpr(op->Duplicate()));
	}


ExprPtr SizeExpr::Duplicate()
	{
	return SetSucc(new SizeExpr(op->Duplicate()));
	}


ExprPtr AddExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new AddExpr(op1_d, op2_d));
	}


ExprPtr AddToExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new AddToExpr(op1_d, op2_d));
	}


ExprPtr SubExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new SubExpr(op1_d, op2_d));
	}


ExprPtr RemoveFromExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new RemoveFromExpr(op1_d, op2_d));
	}


ExprPtr TimesExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new TimesExpr(op1_d, op2_d));
	}


ExprPtr DivideExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new DivideExpr(op1_d, op2_d));
	}


ExprPtr ModExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new ModExpr(op1_d, op2_d));
	}


ExprPtr BoolExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new BoolExpr(tag, op1_d, op2_d));
	}


ExprPtr BitExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new BitExpr(tag, op1_d, op2_d));
	}


ExprPtr EqExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new EqExpr(tag, op1_d, op2_d));
	}


ExprPtr RelExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new RelExpr(tag, op1_d, op2_d));
	}


ExprPtr CondExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	auto op3_d = op3->Duplicate();
	return SetSucc(new CondExpr(op1_d, op2_d, op3_d));
	}

ExprPtr CondExpr::Inline(Inliner* inl)
	{
	op1 = op1->Inline(inl);
	op2 = op2->Inline(inl);
	op3 = op3->Inline(inl);

	return ThisPtr();
	}


ExprPtr RefExpr::Duplicate()
	{
	return SetSucc(new RefExpr(op->Duplicate()));
	}


ExprPtr AssignExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new AssignExpr(op1_d, op2_d, is_init, val));
	}


ExprPtr IndexSliceAssignExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new IndexSliceAssignExpr(op1_d, op2_d, is_init));
	}


ExprPtr IndexExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_l = op2->Duplicate()->AsListExprPtr();
	return SetSucc(new IndexExpr(op1_d, op2_l, is_slice));
	}


ExprPtr IndexExprWhen::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_l = op2->Duplicate()->AsListExprPtr();
	return SetSucc(new IndexExprWhen(op1_d, op2_l, is_slice));
	}


ExprPtr FieldExpr::Duplicate()
	{
	return SetSucc(new FieldExpr(op->Duplicate(), field_name));
	}


ExprPtr HasFieldExpr::Duplicate()
	{
	return SetSucc(new HasFieldExpr(op->Duplicate(), field_name));
	}


ExprPtr RecordConstructorExpr::Duplicate()
	{
	auto op_l = op->Duplicate()->AsListExprPtr();
	return SetSucc(new RecordConstructorExpr(op_l));
	}


ExprPtr TableConstructorExpr::Duplicate()
	{
	auto op_l = op->Duplicate()->AsListExprPtr();

	TypePtr t;
	if ( (type && type->GetName().size() > 0) ||
	     ! op->AsListExpr()->Exprs().empty() )
		t = type;
	else
		// Use a null type rather than the one inferred, to instruct
		// the constructor to again infer the type.
		t = nullptr;

	return SetSucc(new TableConstructorExpr(op_l, nullptr, t, attrs));
	}


ExprPtr SetConstructorExpr::Duplicate()
	{
	auto op_l = op->Duplicate()->AsListExprPtr();

	TypePtr t;
	if ( (type && type->GetName().size() > 0) ||
	     ! op->AsListExpr()->Exprs().empty() )
		t = type;
	else
		// Use a null type rather than the one inferred, to instruct
		// the constructor to again infer the type.
		t = nullptr;

	return SetSucc(new SetConstructorExpr(op_l, nullptr, t, attrs));
	}


ExprPtr VectorConstructorExpr::Duplicate()
	{
	auto op_l = op->Duplicate()->AsListExprPtr();

	if ( op->AsListExpr()->Exprs().empty() )
		return SetSucc(new VectorConstructorExpr(op_l, nullptr));
	else
		return SetSucc(new VectorConstructorExpr(op_l, type));
	}


ExprPtr FieldAssignExpr::Duplicate()
	{
	auto op_dup = op->Duplicate();
	return SetSucc(new FieldAssignExpr(field_name.c_str(), op_dup));
	}


ExprPtr ArithCoerceExpr::Duplicate()
	{
	auto op_dup = op->Duplicate();

	TypeTag tag;

	if ( type->Tag() == TYPE_VECTOR )
		tag = type->AsVectorType()->Yield()->Tag();
	else
		tag = type->Tag();

	return SetSucc(new ArithCoerceExpr(op_dup, tag));
	}


ExprPtr RecordCoerceExpr::Duplicate()
	{
	auto op_dup = op->Duplicate();
	auto rt = GetType()->AsRecordType();
	RecordTypePtr rt_p = {NewRef{}, rt};
	return SetSucc(new RecordCoerceExpr(op_dup, rt_p));
	}


ExprPtr TableCoerceExpr::Duplicate()
	{
	auto op_dup = op->Duplicate();
	auto tt = GetType()->AsTableType();
	TableTypePtr tt_p = {NewRef{}, tt};
	return SetSucc(new TableCoerceExpr(op_dup, tt_p));
	}


ExprPtr VectorCoerceExpr::Duplicate()
	{
	auto op_dup = op->Duplicate();
	auto vt = GetType()->AsVectorType();
	VectorTypePtr vt_p = {NewRef{}, vt};
	return SetSucc(new VectorCoerceExpr(op_dup, vt_p));
	}


ExprPtr ScheduleExpr::Duplicate()
	{
	auto when_d = when->Duplicate();
	auto event_d = event->Duplicate()->AsEventExprPtr();
	return SetSucc(new ScheduleExpr(when_d, event_d));
	}

ExprPtr ScheduleExpr::Inline(Inliner* inl)
	{
	when = when->Inline(inl);
	event = event->Inline(inl)->AsEventExprPtr();

	return ThisPtr();
	}


ExprPtr InExpr::Duplicate()
	{
	auto op1_d = op1->Duplicate();
	auto op2_d = op2->Duplicate();
	return SetSucc(new InExpr(op1_d, op2_d));
	}


ExprPtr CallExpr::Duplicate()
	{
	auto func_d = func->Duplicate();
	auto args_d = args->Duplicate()->AsListExprPtr();
	auto func_type = func->GetType();
	auto in_hook = func_type->AsFuncType()->Flavor() == FUNC_FLAVOR_HOOK;

	return SetSucc(new CallExpr(func_d, args_d, in_hook));
	}

ExprPtr CallExpr::Inline(Inliner* inl)
	{
	auto new_me = inl->CheckForInlining({NewRef{}, this});

	if ( new_me.get() != this )
		return new_me;

	// We're not inlining, but perhaps our elements should be.
	func = func->Inline(inl);

	auto new_args = args->Inline(inl);
	args = {NewRef{}, new_args->AsListExpr()};

	return ThisPtr();
	}


ExprPtr LambdaExpr::Duplicate()
	{
	auto ingr = std::make_unique<function_ingredients>(*ingredients);
	ingr->body = ingr->body->Duplicate();
	return SetSucc(new LambdaExpr(std::move(ingr), outer_ids));
	}

ExprPtr LambdaExpr::Inline(Inliner* inl)
	{
	// Don't inline these, we currently don't get the closure right.
	return ThisPtr();
	}


ExprPtr EventExpr::Duplicate()
	{
	auto args_d = args->Duplicate()->AsListExprPtr();
	return SetSucc(new EventExpr(name.c_str(), args_d));
	}

ExprPtr EventExpr::Inline(Inliner* inl)
	{
	auto el = args->Inline(inl)->AsListExpr();
	args = {NewRef{}, el};

	return ThisPtr();
	}


ExprPtr ListExpr::Duplicate()
	{
	auto new_l = new ListExpr();

	loop_over_list(exprs, i)
		new_l->Append(exprs[i]->Duplicate());

	return SetSucc(new_l);
	}

ExprPtr ListExpr::Inline(Inliner* inl)
	{
	loop_over_list(exprs, i)
		exprs[i] = exprs[i]->Inline(inl).release();

	return ThisPtr();
	}


ExprPtr CastExpr::Duplicate()
	{
	return SetSucc(new CastExpr(op->Duplicate(), type));
	}


ExprPtr IsExpr::Duplicate()
	{
	return SetSucc(new IsExpr(op->Duplicate(), t));
	}


InlineExpr::InlineExpr(ListExprPtr arg_args, IDPList* arg_params,
			StmtPtr arg_body, int _frame_offset, TypePtr ret_type)
: Expr(EXPR_INLINE), args(std::move(arg_args)), body(std::move(arg_body))
	{
	params = arg_params;
	frame_offset = _frame_offset;
	type = ret_type;
	}

bool InlineExpr::IsPure() const
	{
	return args->IsPure() && body->IsPure();
	}

ValPtr InlineExpr::Eval(Frame* f) const
	{
	auto v = eval_list(f, args.get());

	if ( ! v )
		return nullptr;

	int nargs = args->Exprs().length();

	f->Reset(frame_offset + nargs);
	f->IncreaseOffset(frame_offset);

	// Assign the arguments.
	for ( auto i = 0; i < nargs; ++i )
		f->SetElement(i, (*v)[i]);

	auto flow = FLOW_NEXT;
	ValPtr result;
	try
		{
		result = body->Exec(f, flow);
		}

	catch ( InterpreterException& e )
		{
		f->IncreaseOffset(-frame_offset);
		throw;
		}

	f->IncreaseOffset(-frame_offset);

	return result;
	}

ExprPtr InlineExpr::Duplicate()
	{
	auto args_d = args->Duplicate()->AsListExprPtr();
	auto body_d = body->Duplicate();
	return SetSucc(new InlineExpr(args_d, params, body_d, frame_offset,
					type));
	}

TraversalCode InlineExpr::Traverse(TraversalCallback* cb) const
	{
	TraversalCode tc = cb->PreExpr(this);
	HANDLE_TC_EXPR_PRE(tc);

	tc = args->Traverse(cb);
	HANDLE_TC_EXPR_PRE(tc);

	tc = body->Traverse(cb);
	HANDLE_TC_EXPR_PRE(tc);

	tc = cb->PostExpr(this);
	HANDLE_TC_EXPR_POST(tc);
	}

void InlineExpr::ExprDescribe(ODesc* d) const
	{
	if ( d->IsReadable() || d->IsPortable() )
		{
		d->Add("inline(");
		args->Describe(d);
		d->Add("){");
		body->Describe(d);
		d->Add("}");
		}
	else
		{
		args->Describe(d);
		body->Describe(d);
		}
	}


} // namespace zeek::detail
