## TORQUEView - Installation and Usage

TORQUEView is a GUI analysis and control tool for TORQUE. It lets you visually inspect TORQUE node and job lists, and also control operation by allowing you to issue a variety of TORQUE client commands to those nodes or jobs. It was developed using the Qt application framework and needs to be built with QtCreator.


### Installation on Fedora 18

1. yum install -y qt-creator

2. Download/clone the source from github at:
  `https://github.com/davidivanmarsh/TORQUEView`

3. Run qtcreator, and since the torque commands need to be run from root, make sure you run it as root. (This will open a gui). Be sure to add the qt version to the Desktop kit, Tools->Options->Kits. 

4. Open the TORQUEView project file (File->Open File or Project).  The project name is the file that ends in ".pro" (so for TORQUEView, the project name is "TORQUEView.pro")

5. To build TORQUEView, click Build->Build All, or just click on the build icon (hammer) in the lower-left corner of the screen.

6. To run TORQUEView, click the green arrow at the bottom left corner of the screen (for Release), or the green arrow with the circle in it (for Debug).  This will also build it if it isn't built.

7. Your binary will be created at <path_to_source>/../build-TORQUEView-Desktop-[Debug|Release]/TORQUEView.  Later on, you can run the app using "./TORQUEView" from this location if you don't want to keep using QtCreator all the time.


### Installation on Ubuntu 12.04

1. If you already have a built copy:

    sudo apt-add-repository ppa:ubuntu-sdk-team/ppa
    
    sudo apt-get update
    
    sudo apt-get install libqt5widgets5

2. Run TORQUEView

3. To build from source:

    download Qt Creator onto Ubuntu (we are currently using Qt 5.2 and QtCreator version 3.0)  
    follow the same steps as above (starting with Step 2)

### Accessing Remote Servers:
- Make sure you have ssh key encrpytion set up for each of the servers you want to access.



## TORQUEView - A Short User Guide
David Marsh
Last updated:  10/08/2014

The TORQUEView is designed as a TORQUE GUI analysis and diagnostics tool. It lets you visually inspect TORQUE node and job lists, and allows you to control TORQUE operation by issuing a variety of commands to those nodes.

All of TORQUEView's features are powered by execution of TORQUE console commands, accessing either local or remote TORQUE systems (remote commands are issued via ssh calls). Data is extracted from STDOUT output, and is then parsed and displayed on the various tabs and panels.

One of TORQUEView's most unique features is its snapshot capability, which lets you take a "picture" of the condition of a TORQUE system, saving out all node and job state information at a specific point in time. This snapshot can then be loaded and inspected as often as necessary. Even more interesting, a series of snapshots can be taken in succession, and then later replayed to let you watch the changes in the system's nodes and jobs over a period of time.

Note that TORQUEView uses SSH key-based authentication for access to remote servers (password based authentication is not yet supported). You can setup ssh keys with the tool 'ssh-copy-id'.

Also note that TORQUEView does not currently support Cray systems.


### Main Window Features:

#### Data source combobox:

The "Data source" combobox lets you choose which TORQUE data to access and display in the TORQUEView tabs. With this you can analyze either a local host, a remote host, or a snapshot file.  When Remote Host is selected in the combobox, you choose the remote server to analyze from the Remote Server combobox to the immediate right (SSH key-based authentication is required for access to remote servers).  When Snapshot is selected, you select the name of the snapshot file to load from the Snapshots combobox also to the right (this reads in data from a snapshot file and displays it in each of the tab windows).

#### Refresh button and Auto-Refresh checkbox:

The "Refresh" button lets you refresh the display (all the tabs are updated). Using the "Auto-refresh" checkbox, the refresh happens automatically, letting you indicate the refresh interval rate.

#### Snapshot and Replay Snapshot buttons:

TORQUEView's Snapshot feature allows the user to write out the server's current state (i.e, all panel displays) to a file, and then reload this file later on if desired.

The "Snapshot" button lets you take snapshots of the current TORQUE state and TORQUEView screen displays. You can take either a single snapshot, or a series of snapshots (where you indicate the time interval between snapshots).

The "Replay Snapshots" button lets you replay a collection of related snapshots, indicating the time interval between snapshot displays (this value is totally independent of the interval used when generating the snapshot series). Once the replay is started, the user can control the playback with the Pause, Resume, Next, Back, and Cancel buttons on the statusbar.

#### Config button:

The "Config" button on the main panel brings up the Configuration dialog box.  Inside that dialog, the "Remote" and "Local" radio-buttons let you specify the command set for either local or remote TORQUE servers.  If you choose "Remote", the Edit List button lets you create and edit a list of TORQUE servers to analyze. These are used to populate the "Remote host" combobox on the main panel.  The "TORQUEView commands" list shows all the commands currently being exercised when selecting TORQUEView options. Commands are automatically modified when switching between local and remote execution mode, or when selecting new remote servers.  The "Start/Stop MOM format" lets you specify whether it's a "multi-mom" configuration, or whether you want to user "service" commands to start and stop MOMs. The strings used to start or stop a MOM will differ depending on your choice here.


### Tabs:
When you first run TORQUEView, you will notice a set of tab windows at the top of the TORQUEView window, which are titled: heat map, nodes, jobs, qmgr and pbs_server.

#### heat map tab:

The "heat map" tab displays a table of all nodes on the server, showing the current state of each node as a color-coded icon. It can show either nodes without names (labels) or with names. It is most useful when displaying a large number of nodes and their states, letting the user see the state of the entire system at a glance.

![Heat map tab, showing large number of nodes](./screenShots/TorqueTool - Heat Map.png)


#### nodes tab:

The Nodes tab shows three node-related lists. The node list is the primary list in the tab window, and is populated from the output of the "pbsnodes" TORQUE command. The second and third lists show data for individual nodes, which is generated from "pbsnodes" and "momctl -d3" command output.

Depending on which radiobutton you click at the bottom of the screen, "properties" (called "features" in Moab) or "cores" information is also displayed in the node list. When displaying cores, the column headings in the node list indicate the core number ("00" to the max core value), and the column values show job IDs and indicate which job is being run on that core on that node. Double-clicking on a job ID will highlight in yellow all the locations (nodes and cores) where the job is being run.

The "Show nodes running job ID" checkbox is a filter that will show only the nodes running that specific job, and will highlight those job IDs in yellow. Unchecking it displays once again the entire list of nodes, but will continue to highlight the matching job IDs in yellow.

##### node list context menu:
When right-clicking on a node in either the heat map or node list, a context menu lets users:

- start a MOM
- stop of MOM
- mark node as OFFLINE
- clear the OFFLINE designation from the node
- add a note (annotation) to a node
- remove the note (annotation) from the node
- bring up a log viewer to display the log file contents of that node's MOM (it automatically starts executing "tail -f" on the file). To start performing the tail f, click the "Run tail -f" checkbox; to stop, uncheck the box.  Contents of the log viewer's window can be saved to a file with the Save As button. For searching, the log viewer lets you switch between a normal "Find" operation (on the contents of the log viewer window), or if you click the "Use grep" checkbox, it lets you indicate a grep pattern with options (this is done on the original file read in, not the contents of the window.)  Note that TORQUEView requires SSH key based authentication for each remote MOM you want to access. You can setup ssh keys with the tool `ssh-copy-id`.
- highlight all occurrences of the selected job ID across all nodes and cores


#### jobs tab:

The jobs tab shows a list of all the jobs running on the nodes, and is populated from output from the "qstat -R" results. When a job is selected in the list, "qstat -f" results are displayed for that job in a separate panel on the right. If a running job is selected in the jobs list, the "List Hosts Running this Job" button on the lower right will display the host and procs where the job is running (this is essentially a formatted output of the "exec_hosts" and "exec_ports" fields in the "qstat -f" output).

By clicking the "Show Jobs by Queue" checkbox, jobs can be shown heirarchically by queue name.  Also, the job list can be filtered by checking any combination of the Queued, Running, Completed, Hold, or Suspended checkboxes.

Clicking the "Show Nodes Running this Job" button will return the user to the nodes tab and will designate all nodes that are currently running the specified job.


##### job list context menu:
When right-clicking on a job in the list, a context menu lets users:

- run a job
- delete a job
- put a job on hold
- release the hold on a job
- rerun a job

#### qmgr tab:

The qmgr tab provides a GUI interface for the "qmgr" TORQUE command, letting you to issue specific qmgr commands and see the results immediately in the main "qmgr -c 'p s'" output window. As an assist, you can click on a line in the output display (which displays that line in the "submit" edit field below) and modify it before submitting it. 

Finally, by clicking the "Use helps" checkbox, you can use a set of "submit-builder" controls that assist in creating a command line to be submitted. (Click "Insert" to copy the constructed line into the "Command" edit line, and then click the "Submit" button).

#### pbs_server tab:

The pbs_server tab provides an interface to several pbs_server commands and shows their results. On the right is a panel that show the current contents of the "nodes" file, which can be edited if desired by clicking the "Edit" button.

##### Log Viewer:

The pbs_server's "Log Viewer" button brings up a log viewer that lets you do a "tail -f" command on a log file you select. To start performing the "tail -f", click Start; to stop, click the Stop button. Contents of the log viewer's window can be saved to a file with the Save As button.

For searching, the Log Viewer lets you switch between a normal "Find" operation (on the contents of the log viewer window), or if you click the "Use grep" checkbox, it lets you indicate a grep pattern with options (this is done on the original file read in, not the contents of the window.)
