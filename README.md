# MIDIBINZ
### Created by Gabe Lamorie, Josh Eugenio, Lindley Cannon and Tom Mathias
### For James McTigue in DAT450 at Cogswell Polytechnical College

MIDIBINZ takes audio signal in and outputs MIDI information for controlling MIDI CC parameters.

To Install VirtualEnv & Project

Make sure virtualenv is installed in root Python 2.7:
`pip install virtualenv`

Change Directories to A Folder that will contain the virtual environment:
`cd /path/to/venv/folder`

Run virtualenv and give it a name:
`virtualenv name_venv`

Activate environment:
`source name_venv/bin/activate`

Change Directories to where the source code is/will go.  Then if starting with a fresh project:
`git clone https://github.com/JLT-G-2016/MIDIBINZ.git`

Otherwise, if project root already exists, change to that directory and:
`git pull`

Then install dependcies using requirements.txt:
`pip install -r /path/to/project/source/requirements.txt`

Then run `pip list` to make sure all dependencies are installed.
