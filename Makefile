unittest:
	pytest -v tests

flake8:
	flake8 boost_geo_query/

coverage:
	pytest --cov `cd tests && python -c "import os, boost_geo_query; print(os.path.dirname(boost_geo_query.__file__))"` --cov-report=term-missing --cov-report=xml

bandit:
	bandit --recursive boost_geo_query/
	bandit -f json -o bandit-report.json -r boost_geo_query/

safety:
	safety check -r requirements.txt --full-report

pylint:
	pylint boost_geo_query --exit-zero -r n | tee pylint.txt

install:
	pip install -r requirements.txt
	python setup.py develop

install-dev:
	pip install -r requirements-dev.txt
	pip install -r requirements.txt
	python setup.py develop

test-all: unittest flake8 coverage bandit safety pylint
