file(GLOB_RECURSE src
    "./src/*.c"
)
set(include "./include" "./")

idf_component_register(SRCS ${src}
    INCLUDE_DIRS ${include}
    PRIV_INCLUDE_DIRS ".")

lvgl_port_create_c_image("images/logo_bitcoin.png" "images/" "RGB565A8" "NONE")
lvgl_port_create_c_image("images/logo_ethereum.png" "images/" "RGB565A8" "NONE")
lvgl_port_create_c_image("images/wallet_imtoken.png" "images/" "RGB565A8" "NONE")
lvgl_port_create_c_image("images/wallet_metamask.png" "images/" "RGB565A8" "NONE")
lvgl_port_create_c_image("images/wallet_rabby.png" "images/" "RGB565A8" "NONE")

lvgl_port_add_images(${COMPONENT_LIB} "images/")