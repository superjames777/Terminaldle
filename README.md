<h1>Terminaldle</h1>

Terminaldle is Wordle right in your terminal!
Play every single wordle-legal word unlimited*.

## Download and run

Download the archive for your operating system from the [GitHub Releases](https://github.com/superjames777/Terminaldle/releases) page, extract it, and run the `terminaldle` program from the extracted folder. On Windows, run `terminaldle.exe` from PowerShell or Command Prompt. On macOS or Linux, run `./terminaldle` from a terminal. Add `-kwm` to use keyword mode.

## Build from source

You need CMake 3.15 or later and a C++ compiler. Open the repository in VS Code with the CMake Tools extension, configure the project, and build the `terminaldle` target. The executable and its required data files are placed together in the build output directory. The GitHub Actions workflow creates the installable archives for releases.

The GitHub Actions workflow builds Windows, Linux, and universal macOS archives when a version tag such as `v1.0.0` is pushed:

```sh
git tag v1.0.0
git push origin v1.0.0
```

To play, enter a five letter word and hit enter.
You get 6 guesses to figure out what the answer is.

<ul>
    <li>White letters = not in word</li>
    <li>Yellow letters = in word, but in wrong spot</li>
    <li>Green letters = in word and in correct spot</li>
</ul>

Have fun!


*THIS IS VERY EARLY IN DEVELOPMENT. MORE WORDS WILL COME VERY SOON!!!!!!!