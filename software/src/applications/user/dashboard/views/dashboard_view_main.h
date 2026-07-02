#pragma once

#include <gui/view.h>

typedef struct DashboardViewMain DashboardViewMain;

DashboardViewMain* dashboard_view_main_view_alloc(void);
View* dashboard_view_main_get_view(DashboardViewMain* dashboard_view_main);

void dashboard_view_main_view_free(DashboardViewMain* dashboard_view_main);
void dashboard_view_main_update(DashboardViewMain* dashboard_view_main);
