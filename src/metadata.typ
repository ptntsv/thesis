// Single source of truth for the facts printed on the title page and in the
// abstract. Edit the values below; every other file reads them from here, so
// nothing else needs to change.
//
// Anything still wrapped in [square brackets] is a placeholder: replace the
// whole bracketed phrase, brackets included.

#let metadata = (
  // "БАКАЛАВРА" for a bachelor's thesis, "МАГИСТРА" for a master's thesis.
  degree: "БАКАЛАВРА",

  faculty: "Механико-математический факультет",
  department: "Программирования",
  programme: "Математика и компьютерные науки",

  // Family name, first name, patronymic, in that order (nominative case),
  // exactly as it must appear on an official document.
  author: "Путинцев Андрей Алексеевич",

  title: "Исследование и разработка методов контроля качества набора EDTL-требований",

  // Whoever signs off that the thesis is admitted to defence — usually the
  // head of department. `position` is printed as-is, line by line, so split
  // a multi-line title the way it should wrap.
  approver: (
    position: ("[Position, degree]", "[Department, university]"),
    name: "[Approver's name]",
  ),

  supervisor: (
    position: ("[Position, degree]", "[Department, university]"),
    name: "Гаранина Н.О",
  ),

  city: "Новосибирск",
  // `auto` uses the current year; set a literal year (e.g. 2026) to pin it.
  year: auto,
)
