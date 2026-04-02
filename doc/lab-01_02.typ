// #import "@preview/hei-synd-report:0.1.1": *
#import "/doc/metadata.typ": *
#show:make-glossary
#register-glossary(entry-list)

#import "@preview/fractusist:0.1.1":*


//-------------------------------------
// Template config
//

#{
  doc.title = "Environment & Kernel Programming"
  doc.subtitle = [
    Lab 01: Embedded Linux Environment \
    & \
    Lab 02: Linux Kernel Programming
  ]
  doc.logos.tp_main = dragon-curve(
    10,
    step-size: 10,
    stroke-style: stroke(
      paint: gradient.radial(..color.map.rocket),
      thickness: 3pt, join: "round"),
    height: 10cm,
  )
  doc.version = "v0.1.0"
}

#show: report.with(
  option: option,
  doc: doc,
  date: date,
  tableof: tableof,
)
#v(5em)
#infobox()[
  The repository for this labs can be found at the following address:

  #align(center)[https://github.com/Klagarge/MSE-MA-CSEL]
]
#pagebreak()

//-------------------------------------
// Content
//
#include "lab00-env/main.typ"
#pagebreak()
#include "lab01-module/main.typ"

#pagebreak()
= #i18n("appendix-title", lang: option.lang) <sec:appendix>
== Exercices Lab 01

#include "lab01-module/ex01.typ"
#pagebreak()
#include "lab01-module/ex02.typ"
#include "lab01-module/ex03.typ"
#pagebreak()
#include "lab01-module/ex04.typ"
#pagebreak()
#include "lab01-module/ex05.typ"
#include "lab01-module/ex06.typ"
#pagebreak()
#include "lab01-module/ex07.typ"
#include "lab01-module/ex08.typ"

//-------------------------------------
// Glossary
//
// #heading(numbering:none, outlined: false)[] <sec:end>
// #make_glossary(gloss:gloss, title:i18n("gloss-title"))
