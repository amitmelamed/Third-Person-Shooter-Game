# Shooter
Welcome to my 3rd Person Shooter Game project!
*Disclaimer*
The project is NOT YET FINISHED!

![image](https://user-images.githubusercontent.com/88790441/192151889-b10b5bf4-ab54-450a-b41e-d6558a0e70d7.png)  

On this current repository you will find the classed i have made in c++, that helped me creating the functinality of the game.     <br /><br />
#The classes i have made so far are:    <br />
ShooterCharacter->Responsable for the Character functionality, from walking and moving the player camera using the WASD and the mouse to handling the tracing of the weapon fire end location.  
ShooterAnimInstance->Handle all of the Animation logic.  
Item->The Base class for items in the game.  
Weapon->Inherit from the Item class, handle the weapon operation logic.  
Ammo->Inherit from the Item class, handle the ammo operation logic.    <br />

# ShooterCharacter class
The ShooterCharacter class handles all of the character logic, from counting how much ammo we have, to calculate where the bullet we just shot hit.  
I will cover the function logic:  
# Basic Movement
Move the player using WASD keys:<br />
![image](https://user-images.githubusercontent.com/88790441/192152524-26ac5789-c0d1-4827-8627-0246440e25bf.png)<br />
Handle the Mouse/Controller left stick input:  <br />
![image](https://user-images.githubusercontent.com/88790441/192152646-bb25613a-dd74-420e-a411-041db0a177f7.png)<br />
# Crouching and Jumping
We can jump using SPACE key and enter crouching state by C key.  
If we crouch we set our walk speed to lower value.  
if We Crouch and jump we play different animation to smoother Gameplay.  

![image](https://user-images.githubusercontent.com/88790441/192154102-fcaec8c8-a47a-4616-9138-fecc406fa12f.png)

 # Fire logic inside the ShooterCharacter class
 When the FireButton is pressed The FireWeapon function is called: <br />
 ![image](https://user-images.githubusercontent.com/88790441/192152757-f130d562-a40d-4f06-ae9c-77f2053433ce.png) <br />
 but what happenned behined the scene? <br />
 ![image](https://user-images.githubusercontent.com/88790441/192152867-62f21a2f-89f7-4627-a1f9-cef2bbcdb8b0.png)
The FireWeapon function will first check if Can fire our weapon by checking that we are not reloading (ECombatState), and checking that our current equipped gun have ammo using the WeaponHasAmmo() function. <br />
In case we Can fire the bullet The function will do several things: <br />
Play the fire sound. <br />
Send the bullet. <br />
play the gun fire montage. <br />
decrease our ammo by 1. <br />
Spread the crosshairs. <br />
and start the Fire timer, that will not allowed us to fire again untill the timer complete. <br /> <br />
The most interesting part is the SendBullet Function. <br />
This function will calculate using Linear Algebra where the player crosshairs currently aiming at, this point is where the shoot hits and we will spawn there hit partical effect.<br />
After that we will draw a line from the gun barrel to the Crosshair end location and spawn beam effect on this line.<br />
In case that the line from the gun barrel to the Crosshair end location is going trough another objecte we will finish the Beam there.<br />
In this picture we can see the Beam, the partical effects and the bullet animations flying from the gun.<br />
![image](https://user-images.githubusercontent.com/88790441/192153596-94f05150-bd23-428f-b445-832a8d241961.png)<br />
# Aiming down sight logic
When we hold the right mouse button/left trigger on controller we zoom in with our weapon by increasing the Field of view.<br />
![image](https://user-images.githubusercontent.com/88790441/192153678-37a74642-acfb-4ea4-bc05-5844c6951a6e.png)<br />
# Crosshair Spread
The Crosshair spread gets calculated by several parameters:<br />
Is the Player is in air?  
Is the Player crouching?  
Is the Player Aiming?
Is the Player Just Shot the weapon?  
By all of this parameters we calculate the crosshair spread using CalculateCrosshairSpread function, that is called every frame by the Tick function.<br />
![image](https://user-images.githubusercontent.com/88790441/192154017-0a854ab7-80e7-44fe-a989-625b4bb8eda3.png)








