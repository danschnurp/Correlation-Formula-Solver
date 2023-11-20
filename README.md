# kiv-ppr

### Prerequisites

- [VulkanSDK-1.3.268.0](https://vulkan.lunarg.com/sdk/home) (install via cmd in
  Windows (`VulkanSDK-1.2.182.1-Installer.exe --accept-licenses --default-answer --confirm-command install com.lunarg.vulkan.32bit com.lunarg.vulkan.sdl2 com.lunarg.vulkan.glm com.lunarg.vulkan.volk com.lunarg.vulkan.vma com.lunarg.vulkan.debug com.lunarg.vulkan.debug32`))
- compile shaders
- `glslc saxpy.comp -o saxpy.spv`
```
glslc plus.comp -o plus.spv && glslc minus.comp -o minus.spv && \
glslc multiply.comp -o multiply.spv && glslc divide.comp -o divide.spv
```

make sure you are loading with 64bit

### Zadání

- Cílem práce je najít vzorec pro korelaci mezi 3D akcelerometrem a srdečním tepem. Z [https://physionet.org/content/big-ideas-glycemic-wearable/1.1.2/](https://physionet.org/content/big-ideas-glycemic-wearable/1.1.2/) si stáhněte přísloušnou část datových souborů - HR a ACC soubory. Pak napište jednoduchý generátor funkcí s následujícím prototypem:

`double transform(const double acc_x, const double acc_y, const double acc_z);`

- Pro výslednou časovou řadu spočítejte její korelaci se srdečním tepem. Čím menší hodnota, tím lepší korelace. Tím vlastně dostanete fitness funkci, pokud budete generovat těla transformační funkcí evolučním/genetickým algoritmem, jako to např. dělá genetické programovaní nebo gramatická evoluce.

- Celý výpočet spusťte na OpenCL zařízení - tj. nemusíte psát parser výrazů, ale OpenCL driver to udělá za vás. Je vhodné, aby se výpočet míry korelace a transformace odehrál celý na GPU, čímž se pak vyhnete zbytečnému kopírování dat.
Ve finále vyberte transformační funkci s co největší korelací a tu zobrazte v grafu společně se srdeční frekvencí. Graf vygenerujte jako .svg.


poznámky

- preprocess.: normalizace dat (hodnoty mezi 0 a 1), extrapolace chybějících (HR)

- transformační funkci -GPU

- genetický alg. - vektorově (CPU)

- můžeme for each

- build v cmake ?"aspoň c++17"?

- může selhat messenger mezi CPU a GPU v OpenCL
  - využít znalosti problému byzantine generals

[diagram2.pdf](diagram2.pdf)

