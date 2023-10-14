# kiv-ppr

### how to run

https://github.com/KhronosGroup/OpenCL-CLHPP

git clone --recursive https://github.com/KhronosGroup/OpenCL-CLHPP
git clone https://github.com/KhronosGroup/OpenCL-ICD-Loader
git clone https://github.com/KhronosGroup/OpenCL-Headers

todo replace `/Users/danschnurpfeil/CLionProjects/kiv-ppr` with project dir

cmake -D CMAKE_INSTALL_PREFIX=./OpenCL-Headers/install -S ./OpenCL-Headers -B ./OpenCL-Headers/build
cmake --build ./OpenCL-Headers/build --target install -j 6

cmake -D CMAKE_PREFIX_PATH="/Users/danschnurpfeil/CLionProjects/kiv-ppr/OpenCL-Headers/install" -D CMAKE_INSTALL_PREFIX=./OpenCL-ICD-Loader/install -S ./OpenCL-ICD-Loader -B ./OpenCL-ICD-Loader/build
cmake --build ./OpenCL-ICD-Loader/build --target install -j 6

cmake -D CMAKE_PREFIX_PATH="/Users/danschnurpfeil/CLionProjects/kiv-ppr/OpenCL-ICD-Loader/install;/Users/danschnurpfeil/CLionProjects/kiv-ppr/OpenCL-Headers/install" -D CMAKE_INSTALL_PREFIX=./OpenCL-CLHPP/install -S ./OpenCL-CLHPP -B ./OpenCL-CLHPP/build
cmake --build ./OpenCL-CLHPP/build --target install -j 6

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


### opencl

`export CPPFLAGS="-I/usr/local/opt/opencl-clhpp-headers/include"`


`export PKG_CONFIG_PATH="/usr/local/opt/opencl-clhpp-headers/share/pkgconfig"`


