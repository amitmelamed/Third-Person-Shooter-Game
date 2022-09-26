# Shooter
Welcome to my 3rd Person Shooter Game project!  
*Disclaimer*  
The project is NOT YET FINISHED!  
This project is part of the curse "Unreal Engine C++ The Ultimate Shooter Course" created by Stephen Ulibarri.  
Big shoutout to to Stephen, you realy helped me to push my programing skills to another level.

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
 ![image](https://user-images.githubusercontent.com/88790441/192152867-62f21a2f-89f7-4627-a1f9-cef2bbcdb8b0.png)<br />
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
# Hip Turn System
When we are standing still (Same this implemented to crouching) and turn around, we dont want our feet to slide.  
For that reason we implemented a system that turn the upper body, and while the body get to a certein degree compare to the lower body, we are playing a turn in place animaiton.  
![image](https://user-images.githubusercontent.com/88790441/192259142-91807d2c-db62-4a7e-9a15-fffd7686819b.png)
<br />
![image](https://user-images.githubusercontent.com/88790441/192259441-cb94b5d1-1885-4471-aa5c-3e803573f022.png)
<br />
In those picture we can see the different angles between the upper body (that pointed where we are aiming at), and the lower body that standing still.  
# Weapon pointing direction.
In case we are aiming or not, the weapon is pointing in the direction we are aiming at.  
![image](https://user-images.githubusercontent.com/88790441/192260339-be7d5a3b-4fba-49fc-88a4-87c52adf7df8.png)  ![image](https://user-images.githubusercontent.com/88790441/192260608-90aa05f6-ee65-4ff2-923c-58cc61506773.png)  
# Walking System
While we are walking to a different direction, we are blending between different animaiton.  
each animation is different depends on the walking direction.  
All of the walking animation in this specipic blendspace is blending just the lower body, becasuse we want to still be able to use our upper body animations such as reloading, aiming offsets, fire animaiton and more.  
Same thing is applied to crouching system.

# Strifing
If we are walking and moving the Camera to a certein direction, we are blending a strifing animation that make are turn realisticly.  
![image](https://user-images.githubusercontent.com/88790441/192262396-97c3bdb1-7917-46e3-99af-25cdd6d86140.png)











