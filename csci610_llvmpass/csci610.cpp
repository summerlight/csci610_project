#include "llvm/Pass.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct CSCI610 : public FunctionPass {
  static char ID;
  CSCI610() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    if (!isTarget(F)) {
      return false;
    }

    auto* intType = Type::getInt32Ty(F.getContext());
    auto* funcType = FunctionType::get(FunctionType::getVoidTy(F.getContext()), {intType, intType}, false);
    auto* instFunc = F.getParent()->getOrInsertFunction("__instrument", funcType);
    outs() << F.getName() << "\n";
    for (auto& bb : F) {
      auto& front = *bb.begin();
      if (llvm::isa<LandingPadInst>(&front)) {
        continue;
      }
      auto* constant = ConstantInt::getSigned(intType, 0);
      // TODO: allocate function no / label no
      auto* callInst = CallInst::Create(instFunc, {constant, constant});
      callInst->insertBefore(&front);
      outs() << "Basic block (name=" << bb.getName() << ") has "
             << bb.size() << " instructions.\n";
      outs() << bb;
    }
    return false;
  }

  bool isTarget(Function &F) {
    if (F.arg_empty()) {
      return false;
    }
    auto& first_arg = *F.arg_begin();
    auto* first_type = first_arg.getType();
    if (!llvm::isa<llvm::PointerType>(first_type)) {
      return false;
    }
    auto* ptr_type = llvm::cast<llvm::PointerType>(first_type);
    auto* elem_type = ptr_type->getElementType();
    if (!llvm::isa<llvm::StructType>(elem_type)) {
      return false;
    }
    auto* struct_type = llvm::cast<llvm::StructType>(elem_type);
    return struct_type->getName() == "class.stack";
  }
}; // end of struct Hello
}  // end of anonymous namespace

char CSCI610::ID = 0;
static RegisterPass<CSCI610> X("csci610", "CSCI610 Pass",
                             true /* Only looks at CFG */,
                             true /* Analysis Pass */);
