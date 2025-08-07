unittest:
	pytest -v tests

unittest-c:
	rm -rf build/ && \
		mkdir build && \
		cd build && \
		cmake .. -DBUILD_TESTING=ON && \
		make -j && \
		./geo_query_test

ruff:
	ruff check .

coverage:
	pytest --cov `cd tests && python -c "import os, movici_geo_query; print(os.path.dirname(movici_geo_query.__file__))"` --cov-report=term-missing --cov-report=xml

bandit:
	bandit --recursive movici_geo_query/
	bandit -f json -o bandit-report.json -r movici_geo_query/

safety:
	safety check --full-report

pylint:
	pylint movici_geo_query --exit-zero -r n | tee pylint.txt

install:
	pip install -e .

install-dev:
	pip install -e ".[dev]"

test-all: unittest ruff coverage bandit safety

level=patch
export level

bump-version:
	bumpversion --config-file .bumpversion.cfg $(level)
