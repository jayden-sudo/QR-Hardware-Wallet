
# QR Hardware Wallet



### Build your own hardware wallet using affordable, globally available hardware!

The goal of this project is to provide an ***affordable***, ***<u>easy-to-build</u>*** hardware wallet that uses QR code for secure transaction. Our aim is to enable everyone to manage digital assets safely.



## Hardware Supported

The current development is based on the [**ESP32-S3 MCU**](https://www.espressif.com/en/products/socs/esp32-s3), which has enough processing power to handle real-time QR code scanning, making it ideal for a QR-based wallet client.

- MCU

  | MCU      | Support | Test |
  | -------- | ------- | ---- |
  | ESP32-S3 | ✅       | ✅    |
  | ESP32-S2 | ✅       |      |

  - *Specification*: Must have PSRAM and at least 8MB flash.

- Camera

  | Camera | Support | Test |
  | ------ | ------- | ---- |
  | OV2640 | ✅       | ✅    |
  | OV3660 | ✅       |      |
  | GC0308 | ✅        |      |
  | GC032A | ✅        |      |

- Screen

  | Screen            | Support | Test |
  | ----------------- | ------- | ---- |
  | >=`240(W)*320(H)` | ✅       | ✅    |

  - LCD
  
    - | LCD     | Support | Test |
      | ------- | ------- | ---- |
      | ST7735  | ✅       |      |
      | ST7789  | ✅       | ✅    |
      | ST7796  | ✅       |      |
      | ILI9341 | ✅       |      |
      | GC9A01  | ✅       |      |
      |         |         |      |
  
  - Touch Panel
  
    - | TOUCH   | Support | Test |
      | ------- | ------- | ---- |
      | CST816S | ✅       |      |
      | FT5X06  | ✅       | ✅    |
      | FT6X36  | ✅       | ✅    |
      | GT1151  | ✅       |      |
      | GT911   | ✅       |      |
      | TT21100 | ✅       |      |
  
## Project Progress

- [ ] Custom Bootloader

- [x] Multi qr-code input

- **Ethereum Support** (EVM chains)
  - Feature
    - [ ] Legacy Transactions
    - [ ] Access List Transactions (EIP-2930)
    - [x] Fee Market Transactions (EIP-1559)
    - [ ] Blob Transactions (EIP-4844)
    - [x] Sign Personal Message
    - [ ] Sign Typed Data
    - [ ] Sign Typed Data V3
    - [ ] Sign Typed Data V4
    - [ ] Sign Permit
  - Wallet
    - [x] MetaMask
    - [x] imToken
    - [x] Rabby

- **Bitcoin Support**
  
  - [ ] Bitcoin (SegWit)

## Feature Showcase

<div style="display: flex; flex-wrap: wrap; width: 100%">
<img src="docs/images/connect-to-metamask.gif" alt="connect to metamask" style="width: 400px;height: 356px;">
<img src="docs/images/sign-transaction-metamask.gif" alt="sign transaction metamask" style="width: 400px;height: 349px;">
<img src="docs/images/enter-mnemonic.gif" alt="enter mnemonic" style="width: 400px;height: 599px;">
</div>
*Hardware:ESP_S3_EYE-compatible dev board ($6), OV3660-compatible camera ($2), second-hand capacitive touchscreen (240x320, ST7789+FT6X36, $7)*

## How to Build

The project is developed using the VSCode IDE based on the ESP-IDF framework. Compiling the software is incredibly simple:

1. Install the [ESP-IDF (Espressif IoT Development Framework)](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html#ide).
2. Open the project in VSCode IDE.
3. Click the 'ESP-IDF: Build' button to compile the project.

## Thanks

A huge thanks to the following open-source projects, which have been instrumental in the development of this project:

[LVGL](https://github.com/lvgl/lvgl) [uBitcoin](https://github.com/micro-bitcoin/uBitcoin.git) [tinycbor](https://github.com/intel/tinycbor) [airgap-vault](https://github.com/airgap-it/airgap-vault) [KeystoneHQ](https://github.com/KeystoneHQ) [esp32-camera](https://github.com/espressif/esp32-camera) [esp-code-scanner](https://github.com/espressif/) [bc-ur](https://github.com/Blockstream/esp32_bc-ur)

---

Feel free to contribute, open issues, and join the journey to make secure hardware wallets affordable for everyone!
