//
// Created by neo on 25-7-23.
//

#include <mlir/Dialect/SPIRV/IR/SPIRVDialect.h>
#include <mlir/Dialect/SPIRV/IR/SPIRVOps.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Pass/PassManager.h>

int main(int argc, char* argv[]) {
    mlir::MLIRContext context;
    context.loadDialect<mlir::spirv::SPIRVDialect>();
    auto module = mlir::ModuleOp::create(mlir::UnknownLoc::get(&context));
    mlir::OpBuilder builder(&context);
    builder.setInsertionPointToStart(module.getBody());

    auto voidType = builder.getType<mlir::NoneType>();
    auto funcType = mlir::FunctionType::get(builder.getContext(), {}, {voidType});
    auto funcOp   = builder.create<mlir::spirv::FuncOp>(mlir::UnknownLoc::get(&context), "simple_function", funcType);
    funcOp.setPrivate();
    const auto execModelAttr = mlir::spirv::ExecutionModelAttr::get(&context, mlir::spirv::ExecutionModel::GLCompute);
    funcOp->setAttr("main", execModelAttr);

    auto* entryBlock = funcOp.addEntryBlock();
    builder.setInsertionPointToStart(entryBlock);
    builder.create<mlir::spirv::ReturnOp>(mlir::UnknownLoc::get(&context));

    return 0;
}
