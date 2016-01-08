create table package_attributes (
  component text,
  package text,
  key text,
  lineno int4,
  value text
);

create table files (
  component text,
  package text,
  relpath text,
  abspath text,
  size int8
);

create table packages (
  id text,
  component text
);

create or replace function category(p text) returns text as $$
begin
  if substr(p,1,3) = 'lib' then
    return substr(p,1,4);
  else
    return substr(p,1,1);
  end if;
end
$$ language plpgsql;
