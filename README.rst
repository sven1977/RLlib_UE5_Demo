RLlib_UE5_Demo
==============

**TLDR;** An RLlib-ready Unreal Engine 5 project. Uses Anyscale/RLlib's pybind11 utility for C++ simulators
to quickly and conveniently connect from your game to a running RLlib server and start or continue training
an agent policy.



Prerequisites
-------------

Windows
*******

Install git for Win and `git clone [this repo]` to your local computer.

Download `Unreal Engine 5 <https://www.unrealengine.com/>`__, start the Epic Launcher, then install UE5 (version 5.5.4).

Download Visual Studio Community 2022. This repo was created with version 17.12.1, but there is no reason more recent
ones should not work as well.

Download and install `Anaconda <https://www.anaconda.com/>`__ on your PC.

Open a terminal window (as administrator) and run these commands:

.. code-block:: bash

    $ conda create -n ue5_rllib python=3.11
    $ conda activate ue5_rllib

Then pip-install `ray[rllib]` in the new conda environment and link a `rayturbo` git repo into this installation:

.. code-block:: bash

    $ cd rayturbo
    $ python python/ray/setup-dev.py  # link RLlib only


Installation
------------

Windows
+++++++

Open a "File Explorer" window and browse to this downloaded UE5 project repo. Right-click on the `RLlib_UE5_Demo.uproject`
file and select the "Generate Visual Studio Project Files" option. This should create a
new `RLlib_UE5_Demo.sln` file in the same folder or update a possibly already existing one.

Double click on this `.sln` file. Visual Studio should start. You should see something like this:

.. figure:: images/visual_studio_with_project.png

First, setup the build paths of your project. Open the `RLlib_UE5_Demo.Build.cs` file. You can find this file
inside `Games/RLlib_UE5_Demo/Source/RLlib_UE5_Demo/`.

Make sure that the 3 paths for the pybind11 header files, the python library file, and the python include files
are specified correctly. You may need to adjust these depending on your python anaconda setup and pybind11 location.


Try building the project by right clicking on the `RLlib_UE5_Demo` project within the project tree and then click
on "build":

.. figure:: images/visual_studio_build_project.png

Visual Studio + UE5 builds can be painful sometimes due to missing dependencies and cryptic errors. Make sure
you use GPT or any other powerful chat bot to get as much help as possible to get your first build to run through ok.

After Visual Studio reports that the build succeeded without errors, you should now be able to start the game and
run it against RLlib/rayturbo.


Starting the game in the UE5 editor
-----------------------------------

Launch UE5.5.4 through the Epic Launcher app.

.. figure:: images/launch_ue5.png

When the "select project" screen appears, this UE5 project usually doesn't show up yet in any panels, because you
haven't opened it yet on your PC. Instead, enter the location of the ``RLlib_UE5_Demo.uproject`` file at the
root of this repo manually and then click "open".

.. figure:: images/select_uproject_file.png

After some initial loading time, you should see the game project in your editor now.


.. figure:: images/game_in_editor.png

The first map (level) you see should be the `ThirdPersonMap`. This is the only valid map in this project and in case you don't see it, you should switch to it
by clicking on the "Content Browser" button, then navigating to the map file:

.. figure:: images/pick_correct_map.png


Running the game in the UE5 editor
----------------------------------

Press the green play button to start the game. It may take some time to start the game initially, as the RLlibGateway
component actor will try to load the pybind11 dependencies and RLlib modules and even connect to a listening RLlib server.
However, the game should eventually start, even if you don't have any of these components and services setup yet.

Try to navigate with your arrow keys through the small level and collect the blue boxes with the player character.
These should disappear upon collision.
Press `escape` to stop the game.

.. figure:: images/playing_game_in_editor.png


Changing level elements
+++++++++++++++++++++++

You can change the appearance of the level by adding more walls, moving existing ones around, or making the level
itself larger. Note that all wall elements have a tag "wall" and all blue boxes have a tag "bluebox". These tags are
essential for computing the observation vectors. In case you add new elements and need to add either a "wall" or "bluebox" tag,
you can do so through clicking on the added element and then searching for "tag" in the details panel. Then add a new
array element to the Tags array and enter the tag value.

.. figure:: images/change_tags_of_elements.png

Main classes of the game
++++++++++++++++++++++++

RLlibGateway
************

The `RLlibGateway` is a special actor that should only exist once in your game. It's an invisible actor that does not
interact directly with any of the other level components. However, it allows communication with an RLlib sever during
the game and allows for collecting training data and sending this data to RLlib for model updates.

.. figure:: images/configure_rllib_gateway_actor.png

Find the RLlibGateway actor in the map "Outliner" tab, double click it and look at its "Details" panel. In there, you
should find an `RLlib` category, in which you can change the gateway's address and port. Set it to something like
`127.0.0.1` and `5556`, respectively.


Character
*********

Click on the "Content Drawer" on the lower left corner of the editor and click through the directory structure
to: `All/Content/ThirdPerson/Blueprints`, then double click on the `BP_ThirdPerson` blueprint to open it in its own
editor window.

The important tabs are the `Viewport`, in which you see the character and the camera following it during the game,
the `EventGraph` with all the important event blueprint scripts, and some of the blueprint functions, such as `ApplyAction`,
`SetReward`, or `ResetEnv`.

.. figure:: images/bp_character_viewport.png

Now click on `EventGraph` and try navigating and zooming into the "Event Begin Play" event.
You can see that the character class initially zeros out its "Observation" variable (the observation tensor being sent
to the model on each time step to compute the next action) as well as setting a reference to the only `RLlibGateway`
instance in the running game. The character class will need this reference to `RLlibGateway` to compute actions,
set rewards, signal episode termination, and communicate with RLlib.

Walking through the character tick method
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Next, still in the `EventGraph`, zoom in on the `Tick` event. This is the event that fires on each game loop timestep
(aka. "tick"). Here, you can change the character's per-timestep behavior. Right now, the character adds up the time variable
that remembers since when the player vs RLlib has been controlling the character. Then, it adds one to the discrete timestep/tick
counter. This counter is used to figure out, whether an episode should be done (truncated). You can change the maximum timesteps
per episode through the "Episode max timesteps" variable of the character class:

.. figure:: images/character_variable.png

Double click on the variable name to open its "Details" panel, in which you can change its default value. Values somewhere between
1000 and 10000 should make sense here.

If the episode is not done (max timesteps have not been reached), the tick event continues with performing raycasts
from the character into the space in front of the character to compute the next observation tensor.
You can change the exact behavior of the raycast system through the variables:

* Num Rays: How many rays should be cast?
* Trace distance: The length of each ray.
* Vision field: The size of the field of vision (in degrees). For example, 180 means the character scans the environment from 9 o'clock to 3 o'clock.

.. figure:: images/character_raycast_variables.png

Character's observation
~~~~~~~~~~~~~~~~~~~~~~~

The observation tensor of the character is built in the following fashion:

* Each ray is responsible for 5 float32 features. So if you set "Num Rays" to 10, your observation space will be a `Box` of `shape=(50,)`.
* The first feature is whether anything is hit (value of 1.0) or not (value of 0.0).
* The next value is for hitting a wall (1.0 for yes, 0.0 for no). If yes, the third value gives the distance to the wall. If no, the third value will be 0.0.
* The next value is for hitting a blue box (1.0 for yes, 0.0 for no). If yes, the third value gives the distance to the blue box. If no, the third value will be 0.0.

After compiling the next observation tensor, it's time to compute the next action for the character to take, using this
observation. The character has a RLlibGateway reference, which it now uses to call the gateway's GetAction method, passing it
the current observation (and reward; more on rewards, see below). Then the computed action is applied to the character using
the UE5 Character's "Apply Controller Input".

Action space
~~~~~~~~~~~~

The action space of the game is a `Discrete(9)`, where there are 8 actions (up, right, down, left, and all 4 diagonals)
plus one noop action.

Reward function
~~~~~~~~~~~~~~~

The agent is rewarded with +1.0 when colliding with a blue box. You can change this value in the
BP_BlueBox blueprint. Go to the "Content Drawer" and double click the "BP_BlueBox" inside the
`All/Content/ThirdPerson/Blueprints` folder. Then navigate to the `EventGraph` and to the "EventBeginOverlap" event.
There is a method call to the character's `SetReward` function, in which the value of 1.0 is currently hardcoded.

In all other timesteps, the agent receives a small negative reward to motivate it to walk through the level as fast
as possible and collect as many boxes as possible. Change this value inside the Character's blueprint's `Tick` event:

.. figure:: images/character_timestep_reward.png

The logic is currently to receive an overall sum of rewards of -10.0 over the course of one episode, if and only if no box
is collected: `r(t) = -10.0 / [Episode Max Timesteps]`. Change this logic as well as you please.


Blue box
********

Click on the "Content Drawer" on the lower left corner of the editor and click through the directory structure
to: `All/Content/ThirdPerson/Blueprints`, then double click on the `BP_BlueBox` blueprint to open it in its own
editor window.

The important tabs are the `Viewport`, in which you see the blue box static mesh (you can change it to any
shape/mesh you'd like) and the `EventGraph` with the important event blueprint scripts.

.. figure:: images/bp_bluebox_viewport.png

Now click on `EventGraph` and try navigating and zooming into the "Event Begin Play" event.
You can see that each Blue Box actor stores its own starting location initially. This is used when the episode
is reset and all blue boxes are moved back to their starting positions again to start a new episode.

Finally, in the "EventActorBeginOverlap" script, you can see the behavior upon a collision with the character: The box
is made invisible and has its physics behavior revoked, then the reward is set on the character blueprint (where it is
logged with the next "Get Action" call as described above).


Running with an RLlib server
++++++++++++++++++++++++++++

After playing and maybe modifying the game, you can now run an actual RL experiment against an RLlib server.
Run `this RLlib example script <https://github.com/ray-project/ray/blob/master/rllib/examples/envs/env_connecting_to_rllib_w_tcp_client.py>`__:

.. code-block:: bash

    $ python env_connecting_to_rllib_w_tcp_client.py --num-env-runners=0 --port=5556 --no-tune

The script should start and log that the single, local `EnvRunner` (`num-env-runners=0`) is listening on the specified port
5556. Note that if you use more than one `EnvRunner` actors, the provided port will be a "base port" `P` and each
`EnvRunner` actor will get the specific port: `P + env_runner.worker_index`. So if you chose port to be 5556 and have 2 `EnvRunner` actors,
they will listen on ports 5557 and 5558.

Your console should look somewhat like this, before you start the game in UE5:

.. code-block:: bash

    2025-08-27 15:40:32,157	INFO worker.py:1948 -- Started a local Ray instance.
    Waiting for client to connect to port 5556...


Now, click on the green Play button in your UE5 editor to start running the game. Make sure that your `RLlibGateway` actor
in the scene (see above) is set to connect to the same port and to the local network address (`127.0.0.1`).

If you don't take over controlling the character with your arrow keys, you should see this message after about 5 seconds on the
UE5 editor viewport: "RLlib taking over" and the character in the game should start moving around in more or less random
fashion.