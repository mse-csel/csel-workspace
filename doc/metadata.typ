//-------------------------------------
// Document options
//
#let option = (
  type : "final",
  lang : "en",
)

//-------------------------------------
// Metadata of the document
//
#let doc= (
  abbr     : "CSEL",
  logos: (
    tp_topleft  : image("/doc/resources/img/logo-mse.png", width: 9cm),
    tp_topright : image("/doc/resources/img/hesso-logo.svg", width: 4.5cm),
    header      : image("/doc/resources/img/csel.png", width: 2.5cm),
  ),
  authors: (
    (
      name        : "Yann Sierro",
      abbr        : "SIY",
      email       : "yann.sierro@master.hes-so.ch",
    ),
    (
      name        : "Rémi Heredero",
      abbr        : "HER",
      email       : "remi.heredero@hevs.ch",
    ),
  ),
  school: (
    name        : "HES-SO Master",
    major       : "Computer Science",
    orientation : "Embedded",
  ),
  course: (
    name     : "CSEL",
    prof     : "Jacques Supcik",
    semester : "Spring Semester 2026",
  ),
  keywords : ("HES-SO", "Computer Science", "Embedded Systems", "CSEL"),
)

#let date = datetime.today()

//-------------------------------------
// Settings
//
#let tableof = (
  toc: true,
  tof: false,
  tot: false,
  tol: false,
  toe: false,
  maxdepth: 3,
)

#let gloss = true