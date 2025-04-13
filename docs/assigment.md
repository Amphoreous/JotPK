# Assignment 2: Game Prototype
**Due Date:** April 21st

---

## Overview
Your goal is to develop the core gameplay loop of our arcade game with minimal win/lose conditions.

---

## Submission Instructions
One team member per group must upload a single zip file to **Campus Atenea (Assignment 2 - Game Prototype - Submission)** **before Monday, April 21st at 23:59**.  
> **Note:** If more than one build is uploaded, the **last one** will be used. The submission portal will close automatically.

### The zip file must include:
- **Game files**
- **Executable**: Compiled in Release mode with all necessary DLL files.
- **Resources Folder**: A folder containing all media files (e.g., png, wav, ogg).  
  > **Important:** Remove any unnecessary files (e.g., unused code, debug files).
- **README.md**
- **Project Links**:
  - Link to your GitHub repository.
  - Link to a 1-minute (maximum) video (hosted on YouTube or Vimeo) demonstrating all implemented features.
- **Team Details**:
  - List of team members with their GitHub accounts.
  - A short description of the game.
  - Instructions on how to play the game (including detailed controls).
  - A list of implemented features.
- **LICENSE**:
  - Choose an appropriate license for your project (visit [choosealicense.com](https://choosealicense.com/) for guidance).
- **TasksList.pdf**:
  - A table listing tasks performed by each team member along with the dedicated hours.  

#### Example Table Format:
| Team Member | Task Description | Time (hours) |
|-------------|------------------|--------------|
| Name 1      | Task 1           | X            |

### Additional Requirement:
Upload the same zip file to the **Releases** section on GitHub.

#### Suggested Folder Structure:
```
TeamName_GameName_v0.5.zip
├── GameName
├── Assets
│   └── resources
├── .dll files
├── GameName.exe
├── README.md
└── LICENSE
```

---

## Functionality List
All projects must include the following minimum features. Additional functionalities are encouraged.

### 1. Game Loop
- **Initial Screen**: 
  - Project description (subject, degree, university)
  - Team members
  - Tutors
  - Additional details (e.g., webpage/GitHub, Twitter account, team logo, etc.)
- **Title Screen**
- **Gameplay Screen**:
  - At least one level or a part of it
- **Win/Lose Screen**:
  - Option to return to the Title Screen

### 2. General Gameplay
- **Camera Limits**: Ensure the player cannot move outside the visible area.
- **Win/Lose Conditions**: Must be clearly defined.
- **UI Elements**: 
  - Score and lives are displayed (custom fonts are not necessary).
- **Audio**:
  - Background music for each screen.
  - Implement all necessary sound effects.
- **Example Gameplay (Journey of the Prairie King)**:
  - Include 2 rooms with basic player mechanics (movement, basic attack/shoot).
  - Incorporate at least 1 enemy and 1 power-up.

### 3. Video Gameplay
Create a 1-minute video that shows:
- The complete gameplay loop,
- How the game is won and lost,
- Returning to the main menu.

---

## Acceptance Criteria
The submission must meet the following requirements:
- **R1:** The game is submitted on time.
- **R2:** A 1-minute video is provided.
- **R3:** Submitted files adhere to the prescribed naming convention.
- **R4:** The game does not crash during testing (should at least reach win/lose conditions).
- **R5:** The build is correct:
  - Only contains the necessary files,
  - Executable is compiled in Release mode,
  - Proper folder structure,
  - No temporary or debug files.

> **Penalty:** Any issues with points R2, R3, R4, or R5 will result in deductions.

---

## Grading Criteria
- **Features (70%):**
  - All required features must be implemented and function correctly.
  - Include sufficient comments, especially in complex areas.
- **Polish (30%):**
  - Graphical coherence.
  - Quality music and sound effects.
  - Smooth animations and gameplay that resemble the original game concept.
  - Fully commented code.

### Individual Contribution
- The individual efforts of team members will also be evaluated.
- **Important:** If any team member’s contribution is missing from the `TasksList.pdf` (or if tasks/hours are not specified), that member will receive an "NP".