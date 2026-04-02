#import "/doc/metadata.typ": *
#let ln(num) = {
  let str_num = if int(num) < 10 { "0" + str(num) } else { str(num) }
  let lbl = label("lab01:ex" + str_num)
  link(lbl)[Ex 1.#num]
}

= Linux Kernel Programming

== Exercises
#include "ex01.typ"
#include "ex02.typ"
#include "ex03.typ"
#include "ex04.typ"
#include "ex05.typ"
#include "ex07.typ"
