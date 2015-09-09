#pragma once

bool HasEnv(const string& name);

string GetEnv(const string& name);

void SetEnv(const string& name, const string& value, bool overwrite = true);
