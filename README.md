<table>
<tr>

<td style="width:20%"> 
<h1><span style="color: black;font-weight: bolder">SG</span><span style="color: dodgerblue;font-weight: bolder">L</span></h1>
</td>

<td style="width:80%">
<h1><span style="font-weight: lighter">SimpleGPULayer</span></h1>
<h3><span style="font-weight: lighter">High-performance GPGPU library based on Vulkan</span></h3>
</td>

</tr>
</table>

```c++
auto mat1 = ce->CreateMatrix(width, height, data1);
auto mat2 = ce->CreateMatrix(width, height, data2);
auto matOutput = ce->CreateMatrix(width, height);
ce->Seq()->Record(ce->MatMul(mat1, mat2, matOutput))->Eval()->Destroy();
```

### Getting Start

See the [Usage](./doc/getting_start.md) | [Examples](https://gitcode.com/OpenHarmonyPerfSquad/SGL/tree/main/examples) 
