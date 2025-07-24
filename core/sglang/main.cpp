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
#include <mlir/Target/SPIRV/Serialization.h>
#include <mlir/Transforms/Passes.h>

int main(int argc, char* argv[]) {
    mlir::MLIRContext context;
    mlir::DialectRegistry registry;
    registry.insert<mlir::spirv::SPIRVDialect>();
    context.appendDialectRegistry(registry);
    context.loadAllAvailableDialects();

    mlir::OpBuilder builder(&context);

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

    auto workgroupSize = {256, 1, 1};
    builder.create<mlir::spirv::ExecutionModeOp>(
        builder.getUnknownLoc(), funcOp, mlir::spirv::ExecutionMode::LocalSize, workgroupSize);

    auto uintType   = builder.getIntegerType(32);
    auto arrayType  = mlir::spirv::ArrayType::get(uintType, 1024);
    auto structType = mlir::spirv::StructType::get({arrayType}, {});
    auto ptrType    = mlir::spirv::PointerType::get(structType, mlir::spirv::StorageClass::StorageBuffer);

    auto var1Op = builder.create<mlir::spirv::GlobalVariableOp>(
        builder.getUnknownLoc(), ptrType, builder.getStringAttr("inputBuffer1"), nullptr);
    var1Op.setDescriptorSet(0);
    var1Op.setBinding(0);
    var1Op->setAttr("sym_name", builder.getStringAttr("inputBuffer1"));

    auto var2Op = builder.create<mlir::spirv::GlobalVariableOp>(
        builder.getUnknownLoc(), ptrType, builder.getStringAttr("inputBuffer2"), nullptr);
    var2Op.setDescriptorSet(0);
    var2Op.setBinding(1);
    var2Op->setAttr("sym_name", builder.getStringAttr("inputBuffer2"));

    auto var3Op = builder.create<mlir::spirv::GlobalVariableOp>(
        builder.getUnknownLoc(), ptrType, builder.getStringAttr("outputBuffer"), nullptr);
    var3Op.setDescriptorSet(0);
    var3Op.setBinding(2);
    var3Op->setAttr("sym_name", builder.getStringAttr("outputBuffer"));

    auto i32Type           = builder.getI32Type();
    auto uint32Type        = builder.getIntegerType(32, false);
    auto vec3uintType      = mlir::VectorType::get(3, uint32Type);
    auto ptrToGlobalIdType = mlir::spirv::PointerType::get(vec3uintType, mlir::spirv::StorageClass::Input);
    auto globalIdOp        = builder.create<mlir::spirv::GlobalVariableOp>(
        builder.getUnknownLoc(), ptrToGlobalIdType, builder.getStringAttr("gl_GlobalInvocationID"), nullptr);

    globalIdOp->setAttr(
        "builtin", mlir::spirv::BuiltInAttr::get(builder.getContext(), mlir::spirv::BuiltIn::GlobalInvocationId));

    auto globalIdPtr = builder.create<mlir::spirv::AddressOfOp>(builder.getUnknownLoc(),
        mlir::spirv::PointerType::get(vec3uintType, mlir::spirv::StorageClass::Input),
        builder.getStringAttr("gl_GlobalInvocationID"));

    auto globalId = builder.create<mlir::spirv::LoadOp>(builder.getUnknownLoc(), vec3uintType, globalIdPtr.getResult());

    mlir::Block* entryBlock = funcOp.addEntryBlock();
    builder.setInsertionPointToEnd(entryBlock);

    auto indexX = builder.create<mlir::spirv::CompositeExtractOp>(
        builder.getUnknownLoc(), i32Type, globalId.getResult(), builder.getI32ArrayAttr({0}));

    auto addressOfInput1 =
        builder.create<mlir::spirv::AddressOfOp>(builder.getUnknownLoc(), ptrType, var1Op.getSymName());
    auto addressOfInput2 =
        builder.create<mlir::spirv::AddressOfOp>(builder.getUnknownLoc(), ptrType, var2Op.getSymName());
    auto addressOfOutput =
        builder.create<mlir::spirv::AddressOfOp>(builder.getUnknownLoc(), ptrType, var3Op.getSymName());

    auto zero = builder.create<mlir::spirv::ConstantOp>(builder.getUnknownLoc(), i32Type, builder.getI32IntegerAttr(0));

    auto input1AccessChain = builder.create<mlir::spirv::AccessChainOp>(builder.getUnknownLoc(),
        mlir::spirv::PointerType::get(i32Type, mlir::spirv::StorageClass::StorageBuffer), addressOfInput1.getResult(),
        mlir::ValueRange{zero, indexX});

    auto input2AccessChain = builder.create<mlir::spirv::AccessChainOp>(builder.getUnknownLoc(),
        mlir::spirv::PointerType::get(i32Type, mlir::spirv::StorageClass::StorageBuffer), addressOfInput2.getResult(),
        mlir::ValueRange{zero, indexX});

    auto outputAccessChain = builder.create<mlir::spirv::AccessChainOp>(builder.getUnknownLoc(),
        mlir::spirv::PointerType::get(i32Type, mlir::spirv::StorageClass::StorageBuffer), addressOfOutput.getResult(),
        mlir::ValueRange{zero, indexX});

    auto loadInput1 =
        builder.create<mlir::spirv::LoadOp>(builder.getUnknownLoc(), i32Type, input1AccessChain.getResult(),
            mlir::spirv::MemoryAccessAttr::get(builder.getContext(), mlir::spirv::MemoryAccess::Aligned),
            builder.getI32IntegerAttr(4));

    auto loadInput2 =
        builder.create<mlir::spirv::LoadOp>(builder.getUnknownLoc(), i32Type, input2AccessChain.getResult(),
            mlir::spirv::MemoryAccessAttr::get(builder.getContext(), mlir::spirv::MemoryAccess::Aligned),
            builder.getI32IntegerAttr(4));

    auto sum = builder.create<mlir::spirv::IAddOp>(
        builder.getUnknownLoc(), i32Type, loadInput1.getResult(), loadInput2.getResult());

    builder.create<mlir::spirv::StoreOp>(builder.getUnknownLoc(), outputAccessChain, sum,
        mlir::spirv::MemoryAccessAttr::get(builder.getContext(), mlir::spirv::MemoryAccess::Aligned),
        builder.getI32IntegerAttr(4));

    builder.create<mlir::spirv::ReturnOp>(builder.getUnknownLoc());

    spirvModule->dump();

    mlir::SmallVector<uint32_t, 0> spirvBinary;
    serialize(spirvModule, spirvBinary);

    return 0;
}
