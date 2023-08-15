`todo status`          Shows all items of the selected todo list<br>
`todo create [name]`   Makes and selects a new todo list with optional name<br>
`todo add    [text]`   Adds an item to the selected todo list <br>
`todo check  [text]`   Checks the given item, if the text is ambigouos, then all matching items are shown and one item is interactively selected.<br>
`todo toggle [text]`   Similar to check<br>
`todo select [name]`   Selects given todo list, if no name is supplied, all lists are displayed for interactive selection.<br>
`todo dependency [list]:\[index\] [text]` Add a dependency to an item<br>
`todo trigger \[shell script name\] [text]` Makes a new trigger. Whenever the todo programs is ran, all triggers are run. The exit code of the shell script determines whether to check or uncheck the item.<br>
```
$ todo create
$ todo add "Go outside"
$ todo add "Clean bathtub"
$ todo status
unnamed list:
1. [ ] Go outside
2. [ ] Welcom
$ todo check C
$ todo create
$ todo add ""
$ todo select
1. unnamed list
2. unnamed list 2
> 1
$ todo name "Things"
$ todo status
Things:
1. [ ] Go outside
2. [X] Clean bathtub
$ todo dependency Things:1
What should "Go outside" depend on?
> Whether it's nice weather
$ todo status
Things:
1. [ ] Go outside
-> Depends on [ ] Whether it's nice weather
2. [X] Clean bathtub
$ todo trigger get\_weather.sh "Whether it's nice weather"
```
