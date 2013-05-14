//===-- ExpandUtils.cpp - Helper functions for expansion passes -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/NaCl.h"

using namespace llvm;

Instruction *llvm::PhiSafeInsertPt(Use *U) {
  Instruction *InsertPt = cast<Instruction>(U->getUser());
  if (PHINode *PN = dyn_cast<PHINode>(InsertPt)) {
    // We cannot insert instructions before a PHI node, so insert
    // before the incoming block's terminator.  This could be
    // suboptimal if the terminator is a conditional.
    InsertPt = PN->getIncomingBlock(*U)->getTerminator();
  }
  return InsertPt;
}

void llvm::PhiSafeReplaceUses(Use *U, Value *NewVal) {
  if (PHINode *PN = dyn_cast<PHINode>(U->getUser())) {
    // A PHI node can have multiple incoming edges from the same
    // block, in which case all these edges must have the same
    // incoming value.
    BasicBlock *BB = PN->getIncomingBlock(*U);
    for (unsigned I = 0; I < PN->getNumIncomingValues(); ++I) {
      if (PN->getIncomingBlock(I) == BB)
        PN->setIncomingValue(I, NewVal);
    }
  } else {
    U->getUser()->replaceUsesOfWith(U->get(), NewVal);
  }
}