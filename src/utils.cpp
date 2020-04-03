#include "utils.h"
#include "a_color.h"

const size_t FMT_BUFFER_SIZE = 200;
char* fmtbuffer = new char[FMT_BUFFER_SIZE];
const char* format( const char* format, ... ) {
  memset(fmtbuffer, 0, FMT_BUFFER_SIZE);
  va_list va;
  va_start( va, format );
  vsnprintf(fmtbuffer, FMT_BUFFER_SIZE, format, va);
  va_end( va );
  return fmtbuffer;
}
const char* format( const double d ) {
  return format("%.2f", d);
}
const char* format( const int i ) {
  return format("%d", i);
}

const char* svg_attribute(const SVGElement* element, const char * name) {
  const tinyxml2::XMLAttribute* a = element->FindAttribute(name);
  if (a != 0) {
    return a->Value();
  }
  return NULL;
}

void svg_to_file(SVGDocument* doc, FILE* file, const bool compact) {
  tinyxml2::XMLPrinter* printer = new tinyxml2::XMLPrinter(file, compact);
  doc->Print( printer );
  delete(printer);
}

SVGDocument* new_svg_doc(const bool declaration, const bool bom) {
  SVGDocument* doc = new SVGDocument();
  doc->SetBOM( bom );
  if (declaration)
    doc->InsertEndChild( doc->NewDeclaration() );
  return doc;
}

SVGElement* new_svg_element(const char* name, SVGDocument* doc) {
  return doc->NewElement(name);
}

SVGText* new_svg_text(const char* str, SVGDocument* doc, const bool cdata) {
  SVGText* t = doc->NewText(str);
  t->SetCData(cdata);
  return t;
}

void append_element(SVGElement* child, SVGElement* parent) {
  parent->InsertEndChild(child);
}

void prepend_element(SVGElement* child, SVGElement* parent) {
  parent->InsertFirstChild(child);
}

void set_attr(SVGElement* element, const char* name, const char* value) {
  element->SetAttribute(name, value);
}

void set_fill(SVGElement* element, const int col) {
  a_color col_(col);
  if( col_.is_transparent() > 0 ) {
    set_attr(element, "fill", "none");
  } else {
    set_attr(element, "fill", col_.color().c_str());
    set_attr(element, "fill-opacity", col_.opacity().c_str());
  }
}

void set_stroke(SVGElement* element, const double width, const int col, const int type, const int join, const int end) {
  a_color col_(col);
  if( col_.is_transparent() > 0 ) {
    set_attr(element, "stroke", "none");
  } else {
    set_attr(element, "stroke", col_.color().c_str());
    set_attr(element, "stroke-opacity", col_.opacity().c_str());
  }
  if( col_.is_visible() < 1 || width < 0.0001 || type < 0 ) {
    return;
  }

  set_attr(element, "stroke-width", format(width * 72 / 96));

  int lty = type;
  double lwd = width;

  switch (type) {
  case LTY_BLANK:
    break;
  case LTY_SOLID:
    break;
  default:
    std::stringstream os;
    os << (int) lwd * (lty & 15);
    lty = lty >> 4;
    for(int i = 0 ; i < 8 && lty & 15 ; i++) {
      os << ","<< (int) lwd * (lty & 15);
      lty = lty >> 4;
    }
    set_attr(element, "stroke-dasharray", os.str().c_str());
    break;
  }

  switch (join) {
  case GE_ROUND_JOIN: //round
    set_attr(element, "stroke-linejoin", "round");
    break;
  case GE_MITRE_JOIN: //mitre
    set_attr(element, "stroke-linejoin", "miter");
    break;
  case GE_BEVEL_JOIN: //bevel
    set_attr(element, "stroke-linejoin", "bevel");
    break;
  default:
    set_attr(element, "stroke-linejoin", "round");
  break;
  }

  switch (end) {
  case GE_ROUND_CAP:
    set_attr(element, "stroke-linecap", "round");
    break;
  case GE_BUTT_CAP:
    set_attr(element, "stroke-linecap", "butt");
    break;
  case GE_SQUARE_CAP:
    set_attr(element, "stroke-linecap", "square");
    break;
  default:
    set_attr(element, "stroke-linecap", "round");
  break;
  }
}
