
<table>
<tr>

<td width="20%"> 
<h1><span style="color: black">HM</span><span style="color: dodgerblue">C</span></h1>
</td>

<td>

<h1>HMComputeKit</h1>
<h3>High-performance GPGPU library based on Vulkan</h3>

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
