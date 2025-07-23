//
// Created by neo on 25-7-23.
//

#include <mlir/Dialect/SPIRV/IR/SPIRVAttributes.h>
#include <mlir/Dialect/SPIRV/IR/SPIRVDialect.h>
#include <mlir/Dialect/SPIRV/IR/SPIRVEnums.h>
#include <mlir/Dialect/SPIRV/IR/SPIRVOps.h>
#include <mlir/Dialect/SPIRV/IR/SPIRVSerialization.inc>
#include <mlir/Dialect/SPIRV/IR/SPIRVTypes.h>
#include <mlir/Dialect/SPIRV/Transforms/SPIRVConversion.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Pass/PassManager.h>
#include <mlir/Transforms/Passes.h>

int main(int argc, char* argv[]) {
    mlir::MLIRContext context;
    mlir::DialectRegistry registry;
    registry.insert<mlir::spirv::SPIRVDialect>();
    context.appendDialectRegistry(registry);
    context.loadAllAvailableDialects();

    mlir::OpBuilder builder(&context);

    auto moduleOp = mlir::ModuleOp::create(builder.getUnknownLoc());
    builder.setInsertionPointToStart(moduleOp.getBody());


    auto spirvModule = builder.create<mlir::spirv::ModuleOp>(
        builder.getUnknownLoc(), mlir::spirv::AddressingModel::Logical, mlir::spirv::MemoryModel::GLSL450);
    spirvModule->setAttr("vce_triple",
        mlir::spirv::VerCapExtAttr::get(mlir::spirv::Version::V_1_5, {mlir::spirv::Capability::Shader}, {}, &context));

    builder.setInsertionPointToEnd(spirvModule.getBody());
    auto funcType = builder.getFunctionType({}, {});
    auto funcOp   = builder.create<mlir::spirv::FuncOp>(builder.getUnknownLoc(), "main", funcType);

    funcOp->setAttr(mlir::spirv::getEntryPointABIAttrName(),
        builder.getDictionaryAttr({builder.getNamedAttr(
            "main", mlir::spirv::ExecutionModelAttr::get(&context, mlir::spirv::ExecutionModel::GLCompute))}));

    auto workgroupSize = {1, 1, 1};
    builder.create<mlir::spirv::ExecutionModeOp>(
        builder.getUnknownLoc(), funcOp, mlir::spirv::ExecutionMode::LocalSize, workgroupSize);

    auto uintType   = builder.getIntegerType(32);
    auto arrayType  = mlir::spirv::ArrayType::get(uintType, 0);
    auto structType = mlir::spirv::StructType::get({arrayType}, {});

    auto ptrType = mlir::spirv::PointerType::get(structType, mlir::spirv::StorageClass::StorageBuffer);

    auto varOp = builder.create<mlir::spirv::GlobalVariableOp>(
        builder.getUnknownLoc(), ptrType, builder.getStringAttr("buffer"));

    varOp.setDescriptorSet(0);
    varOp.setBinding(0);

    mlir::Block* entryBlock = funcOp.addEntryBlock();
    builder.setInsertionPointToEnd(entryBlock);

    auto i32Type = builder.getI32Type();
    mlir::Value c1 =
        builder.create<mlir::spirv::ConstantOp>(builder.getUnknownLoc(), i32Type, builder.getI32IntegerAttr(1));
    mlir::Value c2 =
        builder.create<mlir::spirv::ConstantOp>(builder.getUnknownLoc(), i32Type, builder.getI32IntegerAttr(2));
    builder.create<mlir::spirv::IAddOp>(builder.getUnknownLoc(), i32Type, c1, c2);
    builder.create<mlir::spirv::ReturnOp>(builder.getUnknownLoc());

    moduleOp->dump();

    mlir::PassManager pm(&context);
    pm.addNestedPass<mlir::spirv::FuncOp>(mlir::createCanonicalizerPass());
    pm.addNestedPass<mlir::spirv::FuncOp>(mlir::createCSEPass());

    // pm.addPass(createLowerToSPIRVPass());
    // pm.addPass(createConvertToSPIRVPass());

    if (failed(pm.run(moduleOp))) {
        llvm::errs() << "Failed to run SPIR-V conversion passes\n";
        return 0;
    }

    llvm::SmallVector<uint32_t, 1024> spirvBinary;

    return 0;
}
