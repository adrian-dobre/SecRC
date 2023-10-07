# SecRC

A simple library to read and control the state of the [Seventilation Sec-RC control panel](https://www.dezentrale-lueftung.com/en/shop/decentralised-ventilation-central-controller-sec-rc/).
Using this library you could transform the basic SEC-RC control panel into a Wi-FI enabled control panel, if paired with something like an ESP-32.

## Some Notes
The Sec-RC control panel is using Zigbee protocol to communicate with the Sevi160-RC ventilation units, however it does not appear to be using a known transport key; or at least I failed to find one that works.
I tried reaching out to the manufacturer and ask for it but I did not get a reply. This is rather unfortunate, as it would've made a smart home integration way cleaner and seems like a missed opportunity.
Maybe the manufacturer plans to release a smart version of this system; although, based on the price of the exiting smart offerings (none currently being available for the newer Sevi160-RC system) I do expect it to cost quite a bit of money.

## How to wire up the panel and configure the library

For device configuration just follow the [basic example](./examples/basic/basic.ino) from the examples folder. You just need to configure the GPIO pin numbers corresponding to each LED/Button for the Sec-RC panel.
All the available commands are listed in the example.

For wiring referece, please check the below picture
Nore: There's also an optional +3V line if you want to power the panel via the microcontroller board (remove the need for a 3V battery).

![Wiring-Reference](./resources/wiring-reference.jpg?raw=true)

It would look something like this:

![Wiring-Example](./resources/wiring-example.jpg?raw=true)