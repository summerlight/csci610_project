#include "llvm/Pass.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
struct CSCI610 : public ModulePass {
  static char ID;
  CSCI610() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {
    int idx = 0;
    for (auto& F : M.functions()) {
      if (isTarget(F)) {
        runOnFunction(F, idx++);
      }
    }
    return true;
  }

  void runOnFunction(Function &F, int funcIdx) {
    auto* intType = Type::getInt32Ty(F.getContext());
    auto* instFuncType = FunctionType::get(FunctionType::getVoidTy(F.getContext()), {intType, intType}, false);
    auto* instFunc = F.getParent()->getOrInsertFunction("__instrument", instFuncType);
    auto* resetFuncType = FunctionType::get(FunctionType::getVoidTy(F.getContext()), {intType}, false);
    auto* resetFunc = F.getParent()->getOrInsertFunction("__instrument_reset", resetFuncType);
    auto* funcNo = ConstantInt::getSigned(intType, funcIdx);
    //outs() << F.getName() << "\n";
    int blockIdx = 0;
    for (auto& bb : F) {
      if (llvm::isa<LandingPadInst>(&bb.front())) {
        continue;
      }
      auto* blockNo = ConstantInt::getSigned(intType, blockIdx++);
      auto* callInst = CallInst::Create(instFunc, {funcNo, blockNo});
      if (llvm::isa<PHINode>(&bb.front())) {
        callInst->insertAfter(&bb.front());
      } else {
        callInst->insertBefore(&bb.front());
      }
      //outs() << "Basic block (name=" << bb.getName() << ") has "
      //       << bb.size() << " instructions.\n";
      //outs() << bb;
    }
    auto& entry = F.getEntryBlock();
    auto* callInst = CallInst::Create(resetFunc, {funcNo});
    callInst->insertBefore(&entry.front());
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
    return struct_type->getName() == "class.stack" || struct_type->getName() == "class.linked_list";
  }
}; // end of struct Hello
}  // end of anonymous namespace

char CSCI610::ID = 0;
static RegisterPass<CSCI610> X("csci610", "CSCI610 Pass",
                             true /* Only looks at CFG */,
                             true /* Analysis Pass */);
