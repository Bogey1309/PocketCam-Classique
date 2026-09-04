**Design Notes**

The enclosure is CAD'ed in Onshape around the size of each component.

Current external dimensions are approximately 55 x 30 x 23mm.

Onshape Document Link:
https://cad.onshape.com/documents/e219cd74eb6f8bbdfaefde86/w/89c30a95217f8900fc007d07/e/92d33215c84a0608a1414959?renderMode=0&uiState=6a9a98a8050614837d4d686d

Design process:
- Measured every component (ESP32-CAM, display, battery, IP5310, switch, button) online
- Built the internal layout first, around where each part physically needs to sit (maximizing space efficiency since i want this to be a keychain cam)
- Designed the enclosure shell around that internal layout
- Polished the enclosure, chamfered it, cut holes for buttons. 

**Design Progression:**
I first measured out the dimensions of the components, then sketched them out on paper. I opened my big box of legos and matched lego pieces to the sketches. Hence I had a good idea of the component sizes. I spent some time trying to arrange the legos in the most space efficient manner.

I then made a rough wiring diagram in draw.io

After that, I opened up onshape and made rough blocks for the components and an enclosure and checked if it all fit. 
I had to make 2 iterations of the enclosure since the first one was not very ergonomic.

I will be using a CH340G to program the ESP32CAM since it does not have a usb port as well.

Attached images:

<img width="1135" height="797" alt="Screenshot 2026-08-21 232245" src="https://github.com/user-attachments/assets/80dc3ece-328b-49cb-8c41-372c28992183" /> _Lego Design_


<img width="919" height="672" alt="Screenshot 2026-08-22 182513" src="https://github.com/user-attachments/assets/3f5f2945-3bcc-4419-935a-6180e13e60fc" />_Rough draw.io Wiring Diagram_


<img width="497" height="432" alt="image" src="https://github.com/user-attachments/assets/7f277352-f75b-491e-8456-f3b2a6a978ec" />
 _Enclosure V1_


<img width="920" height="610" alt="Screenshot 2026-08-23 143825" src="https://github.com/user-attachments/assets/583a7caa-5ed7-4829-b77d-0b359081cca4" />

<img width="508" height="400" alt="Screenshot 2026-08-23 143851" src="https://github.com/user-attachments/assets/ef110a0b-bf27-4964-97ff-8bbdcaf17e15" />

_Enclosure V2_
