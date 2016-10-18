/*
 * Copyright (C) 2016  Marco Gulino <marco@gulinux.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "roi.h"
#include <list>
#include <QDebug>
using namespace std;

DPTR_IMPL(ROIValidator) {
  list<Rule> rules;
  Private(const initializer_list<Rule>& rules) : rules{rules} {}
  static int closest_multiple(int dimension, int factor);
};

ROIValidator::ROIValidator(const initializer_list<Rule>& rules) : dptr(rules)
{
}

ROIValidator::~ROIValidator()
{
}

QRect ROIValidator::validate(const QRect& original) const
{
  QRect result = original;
  for(auto rule: d->rules)
    rule(result);
  return result;
}

ROIValidator::Rule ROIValidator::max_resolution(const QRect& max)
{
  return [max](QRect &roi) {
    roi.setWidth(min(max.width(), roi.width()));
    roi.setHeight(min(max.height(), roi.height()));
  };
}

int ROIValidator::Private::closest_multiple(int dimension, int factor)
{
  return dimension - (dimension % factor);
}


ROIValidator::Rule ROIValidator::height_multiple(int factor)
{
  return [factor](QRect &roi){
    roi.setHeight( Private::closest_multiple(roi.height(), factor) );
  };
}

ROIValidator::Rule ROIValidator::width_multiple(int factor)
{
    return [factor](QRect &roi){
    roi.setWidth( Private::closest_multiple(roi.width(), factor) );
  };
}

ROIValidator::Rule ROIValidator::x_multiple(int factor)
{
  return [factor](QRect &roi){
    roi.setX( Private::closest_multiple(roi.x(), factor) );
  };
}

ROIValidator::Rule ROIValidator::y_multiple(int factor)
{
  return [factor](QRect &roi){
    roi.setY( Private::closest_multiple(roi.y(), factor) );
  };
}
