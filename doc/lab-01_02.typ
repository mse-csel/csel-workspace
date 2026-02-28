// #import "@preview/hei-synd-report:0.1.1": *
#import "@preview/hei-synd-thesis:0.2.3": *
#import "/doc/metadata.typ": *
#import "/doc/resources/glossary.typ": *
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

= Embedded Linux Environment

#lorem(150)

#lorem(50)


//--------------------------------------
#pagebreak()
= Linux Kernel Programming

#lorem(150)

#lorem(50)


//-------------------------------------
// Glossary
//
#heading(numbering:none, outlined: false)[] <sec:end>
#make_glossary(gloss:gloss, title:i18n("gloss-title"))

